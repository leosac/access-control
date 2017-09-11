/*
    Copyright (C) 2014-2016 Leosac

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

#include "modules/websock-api/api/ZoneCRUD.hpp"
#include "core/audit/AuditFactory.hpp"
#include "core/audit/IZoneEvent.hpp"
#include "core/auth/Zone.hpp"
#include "core/auth/Zone_odb.h"
#include "core/auth/serializers/ZoneSerializer.hpp"
#include "modules/websock-api/api/APISession.hpp"
#include "tools/AssertCast.hpp"
#include "tools/db/DBService.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

ZoneCRUD::ZoneCRUD(RequestContext ctx)
    : CRUDResourceHandler(ctx)
{
}

CRUDResourceHandlerUPtr ZoneCRUD::instanciate(RequestContext ctx)
{
    auto instance = CRUDResourceHandlerUPtr(new ZoneCRUD(ctx));
    return instance;
}

boost::optional<json> ZoneCRUD::create_impl(const json &req)
{
    json rep;
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    Auth::ZonePtr new_zone = std::make_shared<Auth::Zone>();
    ZoneJSONSerializer::unserialize(*new_zone, req.at("attributes"),
                                    security_context());
    db->persist(new_zone);

    auto audit = Audit::Factory::ZoneEvent(db, new_zone, ctx_.audit);
    audit->event_mask(Audit::EventType::ZONE_CREATED);
    audit->after(ZoneJSONStringSerializer::serialize(
        *new_zone, SystemSecurityContext::instance()));

    audit->finalize();

    rep["data"] = ZoneJSONSerializer::serialize(*new_zone, security_context());
    t.commit();
    return rep;
}

boost::optional<json> ZoneCRUD::read_impl(const json &req)
{
    json rep;

    using Result = odb::result<Auth::Zone>;
    DBPtr db     = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto zid = req.at("zone_id").get<Auth::ZoneId>();

    if (zid != 0)
    {
        auto zone = ctx_.dbsrv->find_zone_by_id(zid, DBService::THROW_IF_NOT_FOUND);
        rep["data"] = ZoneJSONSerializer::serialize(*zone, security_context());
    }
    else
    {
        Result result     = db->query<Auth::Zone>();
        rep["data"]       = json::array();
        auto current_user = ctx_.session->current_user();

        // fixme: may be rather slow.
        for (const auto &zone : result)
        {
            SecurityContext::ZoneActionParam dap{.zone_id = zone.id()};
            if (ctx_.session->security_context().check_permission(
                    SecurityContext::Action::ZONE_READ, dap))
            {
                rep["data"].push_back(
                    ZoneJSONSerializer::serialize(zone, security_context()));
            }
        }
    }
    t.commit();
    return rep;
}

boost::optional<json> ZoneCRUD::update_impl(const json &req)
{
    json rep;
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto zid = req.at("zone_id").get<Auth::ZoneId>();

    auto zone     = ctx_.dbsrv->find_zone_by_id(zid, DBService::THROW_IF_NOT_FOUND);
    auto zone_odb = assert_cast<Auth::ZonePtr>(zone);
    auto audit    = Audit::Factory::ZoneEvent(db, zone, ctx_.audit);
    audit->event_mask(Audit::EventType::ZONE_UPDATED);
    audit->before(ZoneJSONStringSerializer::serialize(
        *zone, SystemSecurityContext::instance()));

    ZoneJSONSerializer::unserialize(*zone, req.at("attributes"), security_context());

    db->update(zone_odb);
    audit->after(ZoneJSONStringSerializer::serialize(
        *zone, SystemSecurityContext::instance()));

    audit->finalize();
    rep["data"] = ZoneJSONSerializer::serialize(*zone, security_context());
    t.commit();
    return rep;
}

boost::optional<json> ZoneCRUD::delete_impl(const json &req)
{
    auto did = req.at("zone_id").get<Auth::ZoneId>();
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    auto zone     = ctx_.dbsrv->find_zone_by_id(did, DBService::THROW_IF_NOT_FOUND);
    auto zone_odb = assert_cast<Auth::ZonePtr>(zone);
    auto audit    = Audit::Factory::ZoneEvent(db, zone, ctx_.audit);
    audit->event_mask(Audit::EventType::ZONE_DELETED);

    audit->before(ZoneJSONStringSerializer::serialize(
        *zone, SystemSecurityContext::instance()));

    audit->finalize();
    db->erase(zone_odb);
    t.commit();

    return json{};
}

std::vector<CRUDResourceHandler::ActionActionParam>
ZoneCRUD::required_permission(CRUDResourceHandler::Verb verb, const json &req) const
{
    std::vector<CRUDResourceHandler::ActionActionParam> ret;
    SecurityContext::ZoneActionParam zap{};
    try
    {
        zap.zone_id = req.at("zone_id").get<Auth::ZoneId>();
    }
    catch (std::out_of_range &e)
    {
        zap.zone_id = 0;
    }
    switch (verb)
    {
    case Verb::READ:
        ret.emplace_back(SecurityContext::Action::ZONE_READ, zap);
        break;
    case Verb::CREATE:
        ret.emplace_back(SecurityContext::Action::ZONE_CREATE, zap);
        break;
    case Verb::UPDATE:
        ret.emplace_back(SecurityContext::Action::ZONE_UPDATE, zap);
        break;
    case Verb::DELETE:
        ret.emplace_back(SecurityContext::Action::ZONE_DELETE, zap);
        break;
    }
    return ret;
}
