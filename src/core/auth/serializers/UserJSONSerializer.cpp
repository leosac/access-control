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

#include "UserJSONSerializer.hpp"
#include "core/auth/User.hpp"

using namespace Leosac;
using namespace Leosac::Auth;

std::string UserJSONSerializer::to_string(const Auth::User &user,
                                          const SecurityContext &sc)
{
    return to_object(user, sc).dump(4);
}

json UserJSONSerializer::to_object(const Auth::User &user, const SecurityContext &sc)
{
    json memberships = {};
    for (const auto &membership : user.group_memberships())
    {
        SecurityContext::ActionParam ap;
        ap.membership.membership_id = membership->id();
        if (sc.check_permission(SecurityContext::Action::MEMBERSHIP_READ, ap))
        {
            json group_info = {{"id", membership->id()},
                               {"type", "user-group-membership"}};
            memberships.push_back(group_info);
        }
    }
    json serialized = {
        {"id", user.id()},
        {"type", "user"},
        {"attributes",
         {{"version", user.odb_version()},
          {"username", user.username()},
          {"firstname", user.firstname()},
          {"lastname", user.lastname()},
          {"rank", static_cast<int>(user.rank())}}},
        {"relationships", {{"memberships", {{"data", memberships}}}}}};

    SecurityContext::ActionParam ap;
    ap.user.user_id = user.id();
    if (sc.check_permission(SecurityContext::Action::USER_READ_EMAIL, ap))
    {
        serialized["attributes"]["email"] = user.email();
    }
    return serialized;
}
