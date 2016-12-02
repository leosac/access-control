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

#include "api/AccessPointCRUD.hpp"
#include "AccessPoint_odb.h"
#include "Exceptions.hpp"
#include "WSServer.hpp"
#include "api/APISession.hpp"
#include "core/GetServiceRegistry.hpp"
#include "core/audit/AuditFactory.hpp"
#include "core/audit/IAccessPointEvent.hpp"
#include "core/auth/AccessPoint.hpp"
#include "core/auth/User.hpp"
#include "core/auth/serializers/AccessPointSerializer.hpp"
#include "exception/ModelException.hpp"
#include "tools/AssertCast.hpp"
#include "tools/db/DBService.hpp"
#include <cctype>
#include <core/auth/AccessPointService.hpp>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

AccessPointCRUD::AccessPointCRUD(RequestContext ctx)
    : CRUDResourceHandler(ctx)
{
}

CRUDResourceHandlerUPtr AccessPointCRUD::instanciate(RequestContext ctx)
{
    return CRUDResourceHandlerUPtr(new AccessPointCRUD(ctx));
}

boost::optional<json> AccessPointCRUD::create_impl(const json &req)
{
    auto service_ptr =
        get_service_registry().get_service<Auth::AccessPointService>();
    auto controller_module =
        req.at("attributes").at("controller-module").get<std::string>();
    auto ap_backend_ptr = service_ptr->get_backend(controller_module);
    if (ap_backend_ptr)
        return ap_backend_ptr->create(security_context(), ctx_.audit, req);

    // No backend for the requested type of controller module...
    // Throw a general exception that will be mostly useless to the end user
    // but may make sense to devs or system administrators.
    throw LEOSACException(BUILD_STR(
        "Cannot find an AccessPointBackend corresponding to controller-module "
        << controller_module));
}

boost::optional<json> AccessPointCRUD::read_impl(const json &req)
{
    // Read is not forwarded. We simply returns the informations
    // about the base object.
    json rep;

    using Result = odb::result<Auth::AccessPoint>;
    DBPtr db     = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto ap_id = req.at("access_point_id").get<Auth::AccessPointId>();

    auto service_ptr =
        get_service_registry().get_service<Auth::AccessPointService>();

    if (ap_id != 0)
    {
        auto ap = ctx_.dbsrv->find_access_point_by_id(ap_id,
                                                      DBService::THROW_IF_NOT_FOUND);
        rep["data"] = service_ptr->serialize(*ap, security_context());
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
                    service_ptr->serialize(ap, security_context()));
            }
        }
    }
    t.commit();
    return rep;
}

boost::optional<json> AccessPointCRUD::update_impl(const json &req)
{
    // To perform update, we first lookup the base access point object.
    // We then forward the update request to its controller module.
    auto ap_id = req.at("access_point_id").get<Auth::AccessPointId>();
    auto ap =
        ctx_.dbsrv->find_access_point_by_id(ap_id, DBService::THROW_IF_NOT_FOUND);

    auto service_ptr =
        get_service_registry().get_service<Auth::AccessPointService>();
    auto ap_backend_ptr = service_ptr->get_backend(ap->controller_module());
    if (ap_backend_ptr)
        return ap_backend_ptr->update(security_context(), ctx_.audit, req, ap);
    throw LEOSACException(BUILD_STR(
        "Cannot find an AccessPointBackend corresponding to controller-module "
        << ap->controller_module()));
}

boost::optional<json> AccessPointCRUD::delete_impl(const json &req)
{
    // To perform deletion, we first lookup the base access point object.
    // We then forward the deletion request to its controller module.
    auto ap_id = req.at("access_point_id").get<Auth::AccessPointId>();
    auto ap =
        ctx_.dbsrv->find_access_point_by_id(ap_id, DBService::THROW_IF_NOT_FOUND);

    auto service_ptr =
        get_service_registry().get_service<Auth::AccessPointService>();
    auto ap_backend_ptr = service_ptr->get_backend(ap->controller_module());

    if (ap_backend_ptr)
        return ap_backend_ptr->erase(security_context(), ctx_.audit, req, ap);
    throw LEOSACException(BUILD_STR(
        "Cannot find an AccessPointBackend corresponding to controller-module "
        << ap->controller_module()));
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
