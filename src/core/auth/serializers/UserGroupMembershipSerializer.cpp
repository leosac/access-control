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

#include "core/auth/UserGroupMembership.hpp"
#include "UserGroupMembershipSerializer.hpp"
#include "tools/JSONUtils.hpp"

using namespace Leosac;
using namespace Leosac::Auth;

json UserGroupMembershipJSONSerializer::serialize(
    const Auth::UserGroupMembership &membership, const SecurityContext &)
{
    json serialized{};

    auto timestamp           = boost::posix_time::to_time_t(membership.timestamp());
    serialized["id"]         = membership.id();
    serialized["type"]       = "user-group-membership";
    serialized["attributes"] = {{"rank", static_cast<int>(membership.rank())},
                                {"timestamp", timestamp}};
    serialized["relationships"]["user"] = {
        {"data", {{"id", membership.user_id()}, {"type", "user"}}}};
    serialized["relationships"]["group"] = {
        {"data", {{"id", membership.group_id()}, {"type", "group"}}}};

    return serialized;
}

void UserGroupMembershipJSONSerializer::unserialize(Auth::UserGroupMembership &,
                                                    const json &,
                                                    const SecurityContext &)
{
    ASSERT_LOG(false, "Not implemented.");
}

std::string UserGroupMembershipJSONStringSerializer::serialize(
    const Auth::UserGroupMembership &in, const SecurityContext &sc)
{
    return UserGroupMembershipJSONSerializer::serialize(in, sc).dump(4);
}

void UserGroupMembershipJSONStringSerializer::unserialize(
    Auth::UserGroupMembership &out, const std::string &in, const SecurityContext &sc)
{
    auto tmp = json::parse(in);
    UserGroupMembershipJSONSerializer::unserialize(out, tmp, sc);
}
