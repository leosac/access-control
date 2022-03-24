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

#include "UpdateHistory.hpp"
#include "core/GetServiceRegistry.hpp"
#include "core/update/IUpdate.hpp"
#include "core/update/UpdateService.hpp"
#include "core/update/Update_odb.h"
#include "tools/db/DBService.hpp"

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
UpdateHistory::UpdateHistory(RequestContext ctx)
    : MethodHandler(ctx)
{
}

MethodHandlerUPtr UpdateHistory::create(RequestContext ctx)
{
    return std::make_unique<UpdateHistory>(ctx);
}

std::vector<ActionActionParam> UpdateHistory::required_permission(const json &) const
{
    std::vector<ActionActionParam> perm_;
    SecurityContext::ActionParam ap;

    perm_.push_back({SecurityContext::Action::IS_MANAGER, ap});
    return perm_;
}

json UpdateHistory::process_impl(const json &)
{
    json rep;
    rep["data"]     = json::array();
    auto db         = get_service_registry().get_service<DBService>()->db();
    auto update_srv = get_service_registry().get_service<update::UpdateService>();

    odb::transaction t(db->begin());

    using Query = odb::query<update::Update>;
    auto updates =
        db->query<update::Update>(Query::status != update::Status::PENDING);
    for (const auto &update : updates)
    {
        rep["data"].push_back(update_srv->serialize(update, security_context()));
    }

    t.commit();
    return rep;
}
}
}
}
