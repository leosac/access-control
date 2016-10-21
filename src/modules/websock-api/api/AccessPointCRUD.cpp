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

#include "api/AccessPointCRUD.hpp"
#include "AccessPoint_odb.h"
#include "Exceptions.hpp"
#include "api/APISession.hpp"
#include "core/audit/AuditFactory.hpp"
#include "core/audit/IAccessPointEvent.hpp"
#include "core/auth/User.hpp"
#include "core/auth/serializers/AccessPointSerializer.hpp"
#include "exception/ModelException.hpp"
#include "tools/AssertCast.hpp"
#include "tools/db/DBService.hpp"


using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

AccessPointCRUD::AccessPointCRUD(RequestContext ctx)
    : CRUDResourceHandler(ctx)
{
}

CRUDResourceHandlerUPtr AccessPointCRUD::instanciate(RequestContext ctx)
{
    auto instance = CRUDResourceHandlerUPtr(new AccessPointCRUD(ctx));
    return instance;
}

json AccessPointCRUD::create_impl(const json &req)
{
    json rep;
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    Auth::AccessPointPtr new_ap = std::make_shared<Auth::AccessPoint>();
    AccessPointJSONSerializer::unserialize(*new_ap, req.at("attributes"),
                                           security_context());
    db->persist(new_ap);

    auto audit = Audit::Factory::AccessPointEvent(db, new_ap, ctx_.audit);
    audit->event_mask(Audit::EventType::ACCESS_POINT_CREATED);
    audit->after(AccessPointJSONStringSerializer::serialize(
        *new_ap, SystemSecurityContext::instance()));
    audit->finalize();

    rep["data"] = AccessPointJSONSerializer::serialize(*new_ap, security_context());
    t.commit();
    return rep;
}

json AccessPointCRUD::read_impl(const json &req)
{
    json rep;

    using Result = odb::result<Auth::AccessPoint>;
    DBPtr db     = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto ap_id = req.at("access_point_id").get<Auth::AccessPointId>();

    if (ap_id != 0)
    {
        auto ap = ctx_.dbsrv->find_access_point_by_id(ap_id,
                                                      DBService::THROW_IF_NOT_FOUND);
        rep["data"] = AccessPointJSONSerializer::serialize(*ap, security_context());
    }
    else
    {
        Result result     = db->query<Auth::AccessPoint>();
        rep["data"]       = json::array();
        auto current_user = ctx_.session->current_user();

        // fixme: may be rather slow.
        for (const auto &ap : result)
        {
            SecurityContext::AccessPointActionParam aap{.ap_id = ap.id()};
            if (ctx_.session->security_context().check_permission(
                    SecurityContext::Action::ACCESS_POINT_READ, aap))
            {
                rep["data"].push_back(
                    AccessPointJSONSerializer::serialize(ap, security_context()));
            }
        }
    }
    t.commit();
    return rep;
}

json AccessPointCRUD::update_impl(const json &req)
{
    json rep;
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto gid = req.at("access_point_id").get<Auth::AccessPointId>();

    auto ap =
        ctx_.dbsrv->find_access_point_by_id(gid, DBService::THROW_IF_NOT_FOUND);
    auto ap_odb = assert_cast<Auth::AccessPointPtr>(ap);
    auto audit  = Audit::Factory::AccessPointEvent(db, ap, ctx_.audit);
    audit->event_mask(Audit::EventType::ACCESS_POINT_UPDATED);
    audit->before(AccessPointJSONStringSerializer::serialize(
        *ap, SystemSecurityContext::instance()));
    AccessPointJSONSerializer::unserialize(*ap, req.at("attributes"),
                                           security_context());

    db->update(ap_odb);
    audit->after(AccessPointJSONStringSerializer::serialize(
        *ap, SystemSecurityContext::instance()));

    audit->finalize();
    rep["data"] = AccessPointJSONSerializer::serialize(*ap, security_context());
    t.commit();
    return rep;
}

json AccessPointCRUD::delete_impl(const json &req)
{
    auto ap_id = req.at("access_point_id").get<Auth::AccessPointId>();
    DBPtr db   = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    auto ap =
        ctx_.dbsrv->find_access_point_by_id(ap_id, DBService::THROW_IF_NOT_FOUND);
    auto ap_odb = assert_cast<Auth::AccessPointPtr>(ap);
    auto audit  = Audit::Factory::AccessPointEvent(db, ap, ctx_.audit);
    audit->event_mask(Audit::EventType::ACCESS_POINT_DELETED);
    audit->before(AccessPointJSONStringSerializer::serialize(
        *ap, SystemSecurityContext::instance()));

    audit->finalize();
    db->erase(ap_odb);
    t.commit();

    return {};
}

std::vector<CRUDResourceHandler::ActionActionParam>
AccessPointCRUD::required_permission(CRUDResourceHandler::Verb verb,
                                     const json &req) const
{
    std::vector<CRUDResourceHandler::ActionActionParam> ret;
    SecurityContext::AccessPointActionParam aap;
    try
    {
        aap.ap_id = req.at("access_point_id").get<Auth::AccessPointId>();
    }
    catch (std::out_of_range &e)
    {
        aap.ap_id = 0;
    }
    switch (verb)
    {
    case Verb::READ:
        ret.push_back(
            std::make_pair(SecurityContext::Action::ACCESS_POINT_READ, aap));
        break;
    case Verb::CREATE:
        ret.push_back(
            std::make_pair(SecurityContext::Action::ACCESS_POINT_CREATE, aap));
        break;
    case Verb::UPDATE:
        ret.push_back(
            std::make_pair(SecurityContext::Action::ACCESS_POINT_UPDATE, aap));
        break;
    case Verb::DELETE:
        ret.push_back(
            std::make_pair(SecurityContext::Action::ACCESS_POINT_DELETE, aap));
        break;
    }
    return ret;
}
