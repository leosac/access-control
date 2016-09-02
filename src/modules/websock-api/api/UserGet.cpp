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

#include "UserGet.hpp"
#include "Exceptions.hpp"
#include "User_odb.h"
#include "api/APISession.hpp"
#include "conditions/IsCurrentUserAdmin.hpp"
#include "tools/db/DBService.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

UserGet::UserGet(RequestContext ctx)
    : MethodHandler(ctx)
{
}

MethodHandlerUPtr UserGet::create(RequestContext ctx)
{
    auto instance = std::make_unique<UserGet>(ctx);

    auto is_self = [ptr = instance.get()](const json &req)
    {
        auto uid = req.at("user_id").get<Auth::UserId>();
        return ptr->ctx_.session->current_user_id() == uid;
    };

    instance->add_conditions_or(
        []() { throw PermissionDenied(); },
        Conditions::wrap(Conditions::IsCurrentUserAdmin(ctx)), is_self);
    return instance;
}

json UserGet::process_impl(const json &req)
{
    json rep;

    using query = odb::query<Auth::User>;
    DBPtr db    = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    odb::session s;
    auto uid = req.at("user_id").get<Auth::UserId>();

    Auth::UserPtr user = db->query_one<Auth::User>(query::id == uid);
    if (user)
    {
        json memberships = {};
        for (const auto &membership : user->group_memberships())
        {
            json group_info = {{"id", membership->id()},
                               {"type", "user-group-membership"}};
            memberships.push_back(group_info);
        }
        rep["data"] = {
            {"id", user->id()},
            {"type", "user"},
            {"attributes",
             {{"username", user->username()},
              {"firstname", user->firstname()},
              {"lastname", user->lastname()}}},
            {"relationships", {{"memberships", {{"data", memberships}}}}}};
    }
    else
        throw EntityNotFound(uid, "user");
    t.commit();
    return rep;
}
