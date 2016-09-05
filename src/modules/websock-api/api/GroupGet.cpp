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

#include "GroupGet.hpp"
#include "Exceptions.hpp"
#include "Group_odb.h"
#include "api/APISession.hpp"
#include "conditions/IsCurrentUserAdmin.hpp"
#include "conditions/IsInGroup.hpp"
#include "tools/db/DBService.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

GroupGet::GroupGet(RequestContext ctx)
    : MethodHandler(ctx)
{
}

MethodHandlerUPtr GroupGet::create(RequestContext ctx)
{
    auto instance = std::make_unique<GroupGet>(ctx);

    auto is_in_group = [ ctx = ctx, ptr = instance.get() ](const json &req)
    {
        using namespace Conditions;
        auto gid = req.at("group_id").get<Auth::GroupId>();
        return wrap(IsInGroup(ctx, gid))(req);
    };

    instance->add_conditions_or(
        []() { throw PermissionDenied(); },
        Conditions::wrap(Conditions::IsCurrentUserAdmin(ctx)), is_in_group);
    return std::move(instance);
}

json GroupGet::process_impl(const json &req)
{
    json rep;

    using query = odb::query<Auth::Group>;
    DBPtr db    = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    odb::session s;
    auto gid = req.at("group_id").get<Auth::GroupId>();

    Auth::GroupPtr group = db->query_one<Auth::Group>(query::id == gid);
    if (group)
    {
        json memberships = {};
        for (const auto &membership : group->user_memberships())
        {
            json group_info = {{"id", membership->id()},
                               {"type", "user-group-membership"}};
            memberships.push_back(group_info);
        }
        rep["data"] = {{"id", group->id()},
                       {"type", "group"},
                       {"attributes",
                        {
                            {"name", group->name()},
                        }},
                       {"relationships", {{"members", {{"data", memberships}}}}}};
    }
    t.commit();
    return rep;
}
