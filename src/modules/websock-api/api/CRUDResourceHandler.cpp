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

#include "api/CRUDResourceHandler.hpp"
#include "Exceptions.hpp"
#include "WSServer.hpp"
#include "exception/PermissionDenied.hpp"
#include "tools/log.hpp"
#include <boost/algorithm/string/predicate.hpp>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

ICRUDResourceHandler::Verb
ICRUDResourceHandler::verb_from_request_type(const std::string &req)
{
    if (boost::algorithm::ends_with(req, ".read"))
        return Verb::READ;
    else if (boost::algorithm::ends_with(req, ".create"))
        return Verb::CREATE;
    else if (boost::algorithm::ends_with(req, ".update"))
        return Verb::UPDATE;
    else if (boost::algorithm::ends_with(req, ".delete"))
        return Verb::DELETE;
    else
    {
        ASSERT_LOG(0, "Invalid request type {" << req
                                               << "} for CRUD resource handler");
        throw LEOSACException("Should not be here");
    }
}

void ICRUDResourceHandler::enforce_permission(
    const std::vector<ActionActionParam> &permissions)
{
    auto &security_ctx = security_context();
    for (const auto &action_and_param : permissions)
    {
        if (!security_ctx.check_permission(action_and_param.first,
                                           action_and_param.second))
        {
            throw PermissionDenied();
        }
    }
}

boost::optional<json> ICRUDResourceHandler::process(const ClientMessage &msg)
{
    auto perms = required_permission(verb_from_request_type(msg.type), msg.content);
    switch (verb_from_request_type(msg.type))
    {
    case Verb::READ:
        enforce_permission(perms);
        return read_impl(msg.content);
    case Verb::CREATE:
        enforce_permission(perms);
        return create_impl(msg.content);
    case Verb::UPDATE:
        enforce_permission(perms);
        return update_impl(msg.content);
    case Verb::DELETE:
        enforce_permission(perms);
        return delete_impl(msg.content);
    }
    ASSERT_CANNOT_BE_HERE(".");
}

CRUDResourceHandler::CRUDResourceHandler(RequestContext ctx)
    : ctx_(ctx)
{
}

CRUDResourceHandlerUPtr CRUDResourceHandler::instanciate(RequestContext)
{
    return nullptr;
}

UserSecurityContext &CRUDResourceHandler::security_context() const
{
    auto wsc =
        dynamic_cast<UserSecurityContext *>(&ctx_.session->security_context());
    ASSERT_LOG(wsc, "SecurityContext has unexpected type.");
    return *wsc;
}

ExternalCRUDResourceHandler::ExternalCRUDResourceHandler(ModuleRequestContext ctx)
    : ctx_(ctx)
{
}

UserSecurityContext &ExternalCRUDResourceHandler::security_context() const
{
    return *ctx_.security_ctx;
}
