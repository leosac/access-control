/*
    Copyright (C) 2014-2016 Islog

    This file is part of Leosac.

    Leosac is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leosac is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "api/DoorCRUD.hpp"
#include "Door_odb.h"
#include "Exceptions.hpp"
#include "api/APISession.hpp"
#include "core/audit/AuditFactory.hpp"
#include "core/audit/IDoorEvent.hpp"
#include "core/auth/User.hpp"
#include "core/auth/serializers/DoorSerializer.hpp"
#include "exception/ModelException.hpp"
#include "tools/AssertCast.hpp"
#include "tools/db/DBService.hpp"


using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

DoorCRUD::DoorCRUD(RequestContext ctx)
    : CRUDResourceHandler(ctx)
{
}

CRUDResourceHandlerUPtr DoorCRUD::instanciate(RequestContext ctx)
{
    auto instance = CRUDResourceHandlerUPtr(new DoorCRUD(ctx));
    return instance;
}

json DoorCRUD::create_impl(const json &req)
{
    json rep;
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    Auth::DoorPtr new_door = std::make_shared<Auth::Door>();
    DoorJSONSerializer::unserialize(*new_door, req.at("attributes"),
                                    security_context());
    db->persist(new_door);

    auto audit = Audit::Factory::DoorEvent(db, new_door, ctx_.audit);
    audit->event_mask(Audit::EventType::DOOR_CREATED);
    audit->after(DoorJSONStringSerializer::serialize(
        *new_door, SystemSecurityContext::instance()));
    audit->finalize();

    rep["data"] = DoorJSONSerializer::serialize(*new_door, security_context());
    t.commit();
    return rep;
}

json DoorCRUD::read_impl(const json &req)
{
    json rep;

    using Result = odb::result<Auth::Door>;
    DBPtr db     = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto did = req.at("door_id").get<Auth::DoorId>();

    if (did != 0)
    {
        auto door = ctx_.dbsrv->find_door_by_id(did, DBService::THROW_IF_NOT_FOUND);
        rep["data"] = DoorJSONSerializer::serialize(*door, security_context());
    }
    else
    {
        Result result     = db->query<Auth::Door>();
        rep["data"]       = json::array();
        auto current_user = ctx_.session->current_user();

        // fixme: may be rather slow.
        for (const auto &door : result)
        {
            SecurityContext::DoorActionParam dap{.door_id = door.id()};
            if (ctx_.session->security_context().check_permission(
                    SecurityContext::Action::DOOR_READ, dap))
            {
                rep["data"].push_back(
                    DoorJSONSerializer::serialize(door, security_context()));
            }
        }
    }
    t.commit();
    return rep;
}

json DoorCRUD::update_impl(const json &req)
{
    json rep;
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto gid = req.at("door_id").get<Auth::DoorId>();

    auto door     = ctx_.dbsrv->find_door_by_id(gid, DBService::THROW_IF_NOT_FOUND);
    auto door_odb = assert_cast<Auth::DoorPtr>(door);
    auto audit    = Audit::Factory::DoorEvent(db, door, ctx_.audit);
    audit->event_mask(Audit::EventType::DOOR_UPDATED);
    audit->before(DoorJSONStringSerializer::serialize(
        *door, SystemSecurityContext::instance()));
    DoorJSONSerializer::unserialize(*door, req.at("attributes"), security_context());

    db->update(door_odb);
    audit->after(DoorJSONStringSerializer::serialize(
        *door, SystemSecurityContext::instance()));

    audit->finalize();
    rep["data"] = DoorJSONSerializer::serialize(*door, security_context());
    t.commit();
    return rep;
}

json DoorCRUD::delete_impl(const json &req)
{
    auto did = req.at("door_id").get<Auth::DoorId>();
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    auto door     = ctx_.dbsrv->find_door_by_id(did, DBService::THROW_IF_NOT_FOUND);
    auto door_odb = assert_cast<Auth::DoorPtr>(door);
    auto audit    = Audit::Factory::DoorEvent(db, door, ctx_.audit);
    audit->event_mask(Audit::EventType::DOOR_DELETED);

    db->erase(door_odb);
    t.commit();

    return {};
}

std::vector<CRUDResourceHandler::ActionActionParam>
DoorCRUD::required_permission(CRUDResourceHandler::Verb verb, const json &req) const
{
    std::vector<CRUDResourceHandler::ActionActionParam> ret;
    SecurityContext::DoorActionParam dap;
    try
    {
        dap.door_id = req.at("door_id").get<Auth::DoorId>();
    }
    catch (std::out_of_range &e)
    {
        dap.door_id = 0;
    }
    switch (verb)
    {
    case Verb::READ:
        ret.push_back(std::make_pair(SecurityContext::Action::DOOR_READ, dap));
        break;
    case Verb::CREATE:
        ret.push_back(std::make_pair(SecurityContext::Action::DOOR_CREATE, dap));
        break;
    case Verb::UPDATE:
        ret.push_back(std::make_pair(SecurityContext::Action::DOOR_UPDATE, dap));
        break;
    case Verb::DELETE:
        ret.push_back(std::make_pair(SecurityContext::Action::DOOR_DELETE, dap));
        break;
    }
    return ret;
}
