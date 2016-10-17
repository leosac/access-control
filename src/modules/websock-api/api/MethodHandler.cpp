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

#include "api/MethodHandler.hpp"
#include "Exceptions.hpp"
#include "WSServer.hpp"
#include "exception/PermissionDenied.hpp"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

json MethodHandler::process(const ClientMessage &msg)
{
    auto &security_ctx = ctx_.session->security_context();
    for (const auto &action_and_param : required_permission(msg.content))
    {
        if (!security_ctx.check_permission(action_and_param.first,
                                           action_and_param.second))
        {
            throw PermissionDenied();
        }
    }
    return process_impl(msg.content);
}

UserSecurityContext &MethodHandler::security_context()
{
    auto wsc =
        dynamic_cast<UserSecurityContext *>(&ctx_.session->security_context());
    ASSERT_LOG(wsc, "SecurityContext has unexpected type.");
    return *wsc;
}
