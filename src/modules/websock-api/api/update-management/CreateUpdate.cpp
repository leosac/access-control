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

#include "CreateUpdate.hpp"
#include "core/GetServiceRegistry.hpp"
#include "core/update/IUpdate.hpp"
#include "core/update/UpdateService.hpp"

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
CreateUpdate::CreateUpdate(RequestContext ctx)
    : MethodHandler(ctx)
{
}

MethodHandlerUPtr CreateUpdate::create(RequestContext ctx)
{
    return std::make_unique<CreateUpdate>(ctx);
}

std::vector<ActionActionParam> CreateUpdate::required_permission(const json &) const
{
    std::vector<ActionActionParam> perm_;
    SecurityContext::ActionParam ap;

    perm_.push_back({SecurityContext::Action::IS_MANAGER, ap});
    return perm_;
}

json CreateUpdate::process_impl(const json &req)
{
    auto srv_ptr = get_service_registry().get_service<update::UpdateService>();
    ASSERT_LOG(srv_ptr, "Cannot retrieve UpdateService.");
    update::IUpdatePtr update =
        srv_ptr->create_update(req.at("descriptor_uuid"), exec_context());

    json ret;
    if (update)
        ret["data"] = srv_ptr->serialize(*update, security_context());
    return ret;
}
}
}
}
