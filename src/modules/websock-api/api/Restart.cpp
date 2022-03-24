/*
    Copyright (C) 2014-2022 Leosac

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

#include "modules/websock-api/api/Restart.hpp"
#include "core/CoreAPI.hpp"
#include "core/CoreUtils.hpp"
#include "modules/websock-api/WSServer.hpp"

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
Restart::Restart(RequestContext ctx)
    : MethodHandler(ctx)
{
}

MethodHandlerUPtr Restart::create(RequestContext rc)
{
    return std::make_unique<Restart>(rc);
}

std::vector<ActionActionParam> Restart::required_permission(const json &) const
{
    std::vector<ActionActionParam> perm;
    perm.push_back({SecurityContext::Action::RESTART_SERVER, {}});
    return perm;
}

json Restart::process_impl(const json &)
{
    ctx_.server.core_utils()->core_api().restart_server();
    return json{};
}
}
}
}
