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
#include "WSServer.hpp"
#include "api/APISession.hpp"
#include "core/audit/AuditFactory.hpp"
#include "core/audit/IAccessPointEvent.hpp"
#include "core/auth/AccessPoint.hpp"
#include "core/auth/User.hpp"
#include "core/auth/serializers/AccessPointSerializer.hpp"
#include "exception/ModelException.hpp"
#include "tools/AssertCast.hpp"
#include "tools/db/DBService.hpp"
#include <cctype>

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
    forward_to_impl_module(
        req, req.at("attributes").at("controller-module").get<std::string>());
    return boost::none;
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

boost::optional<json> AccessPointCRUD::update_impl(const json &req)
{
    // To perform deletion, we first lookup the base access point object.
    // We then forward the update request to its controller module.
    auto ap_id = req.at("access_point_id").get<Auth::AccessPointId>();
    DBPtr db   = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    auto ap =
        ctx_.dbsrv->find_access_point_by_id(ap_id, DBService::THROW_IF_NOT_FOUND);
    forward_to_impl_module(req, ap->controller_module());
    return boost::none;
}

boost::optional<json> AccessPointCRUD::delete_impl(const json &req)
{
    // To perform deletion, we first lookup the base access point object.
    // We then forward the deletion request to its controller module.
    auto ap_id = req.at("access_point_id").get<Auth::AccessPointId>();
    DBPtr db   = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    auto ap =
        ctx_.dbsrv->find_access_point_by_id(ap_id, DBService::THROW_IF_NOT_FOUND);
    forward_to_impl_module(req, ap->controller_module());
    return boost::none;
}

void AccessPointCRUD::forward_to_impl_module(const json &req, const std::string &mod)
{
    // We we forward the call to the proper implementation module
    // endpoint.

    // Note that we have currently no way to make sure the module
    // we are sending to exists. If it doesn't, Leosac may assert.

    std::string controller_module = mod;
    std::transform(controller_module.begin(), controller_module.end(),
                   controller_module.begin(), ::tolower);
    std::string new_type =
        "module." + controller_module + "." + ctx_.original_msg.type;

    json reconstructed = {{"uuid", ctx_.original_msg.uuid},
                          {"type", new_type},
                          {"content", ctx_.original_msg.content}};

    // We need to emulate the behavior of WSServer::dispatch_request, so that
    // the target module is able to handle this correctly.
    zmqpp::message zmq_msg;
    zmq_msg << ctx_.audit->id() << ctx_.session->connection_identifier()
            << reconstructed.dump(4);

    std::transform(controller_module.begin(), controller_module.end(),
                   controller_module.begin(), ::toupper);
    ctx_.server.send_to_module(controller_module, std::move(zmq_msg));
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
