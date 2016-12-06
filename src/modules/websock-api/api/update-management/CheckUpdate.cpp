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

#include "CheckUpdate.hpp"
#include "core/GetServiceRegistry.hpp"
#include "core/update/IUpdate.hpp"
#include "core/update/UpdateService.hpp"

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
CheckUpdate::CheckUpdate(RequestContext ctx)
    : MethodHandler(ctx)
{
}

MethodHandlerUPtr CheckUpdate::create(RequestContext ctx)
{
    return std::make_unique<CheckUpdate>(ctx);
}

std::vector<ActionActionParam>
CheckUpdate::required_permission(const json &req) const
{
    std::vector<ActionActionParam> perm_;
    SecurityContext::ActionParam ap;

    perm_.push_back({SecurityContext::Action::IS_MANAGER, ap});
    return perm_;
}

json CheckUpdate::process_impl(const json &req)
{
    auto srv_ptr = get_service_registry().get_service<update::UpdateService>();
    ASSERT_LOG(srv_ptr, "Cannot retrieve UpdateService.");
    json ret;
    ret["data"] = json::array();

    auto updates = srv_ptr->check_update();
    for (const auto &update : updates)
    {
        ret["data"].push_back(srv_ptr->serialize(*update, security_context()));
    }

    return ret;
}
}
}
}