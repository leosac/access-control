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

#include "MembershipGet.hpp"
#include "Exceptions.hpp"
#include "UserGroupMembership_odb.h"
#include "User_odb.h"
#include "api/APISession.hpp"
#include "conditions/IsCurrentUserAdmin.hpp"
#include "core/auth/UserGroupMembership.hpp"
#include "tools/db/DBService.hpp"
#include <conditions/IsInGroup.hpp>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

MembershipGet::MembershipGet(RequestContext ctx)
    : MethodHandler(ctx)
{
}

MethodHandlerUPtr MembershipGet::create(RequestContext ctx)
{
    auto instance = std::make_unique<MembershipGet>(ctx);

    // We are allowed to query Membership information, if:
    //    + We are a member of the group the membership is about
    //      (aka we can see other group members' membership info
    //    + We are an administrator
    //    + The membership is about the current user.

    auto is_about_self = [ ctx = ctx, ptr = instance.get() ](const json &req)
    {
        using namespace Auth;
        auto mid = req.at("membership_id").get<Auth::UserGroupMembershipId>();
        odb::transaction t(ctx.dbsrv->db()->begin());
        UserGroupMembershipPtr membership =
            ctx.dbsrv->db()->load<UserGroupMembership>(mid);

        return membership &&
               membership->user().object_id() == ctx.session->current_user_id();
    };

    auto is_about_my_group = [ ctx = ctx, ptr = instance.get() ](const json &req)
    {
        using namespace Auth;
        auto mid = req.at("membership_id").get<Auth::UserGroupMembershipId>();
        odb::transaction t(ctx.dbsrv->db()->begin());
        UserGroupMembershipPtr membership =
            ctx.dbsrv->db()->load<UserGroupMembership>(mid);
        t.commit();

        if (membership)
        {
            return Conditions::IsInGroup(ctx, membership->group().object_id())();
        }
        return false;
    };

    instance->add_conditions_or(
        []() { throw PermissionDenied(); },
        Conditions::wrap(Conditions::IsCurrentUserAdmin(ctx)), is_about_my_group,
        is_about_self);
    return std::move(instance);
}

json MembershipGet::process_impl(const json &req)
{
    json rep;

    using query = odb::query<Auth::UserGroupMembership>;
    DBPtr db    = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto mid = req.at("membership_id").get<Auth::UserGroupMembershipId>();

    Auth::UserGroupMembershipPtr membership =
        db->query_one<Auth::UserGroupMembership>(query::id == mid);
    if (membership)
    {
        auto timestamp      = boost::posix_time::to_time_t(membership->timestamp());
        rep["data"]         = {};
        rep["data"]["id"]   = membership->id();
        rep["data"]["type"] = "user-group-membership";
        rep["data"]["attributes"] = {{"rank", static_cast<int>(membership->rank())},
                                     {"timestamp", timestamp}};
        rep["data"]["relationships"]["user"] = {
            {"data", {{"id", membership->user().object_id()}, {"type", "user"}}}};
        rep["data"]["relationships"]["group"] = {
            {"data", {{"id", membership->group().object_id()}, {"type", "group"}}}};
    }
    else
        throw EntityNotFound(mid, "user-group-membership");
    t.commit();
    return rep;
}
