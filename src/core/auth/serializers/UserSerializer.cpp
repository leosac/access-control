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

#include "core/auth/serializers/UserSerializer.hpp"
#include "Credential_odb.h"
#include "ScheduleMapping_odb.h"
#include "Schedule_odb.h"
#include "core/SecurityContext.hpp"
#include "core/auth/User.hpp"
#include "core/credentials/CredentialFwd.hpp"
#include "core/credentials/serializers/PolymorphicCredentialSerializer.hpp"
#include "tools/Conversion.hpp"
#include "tools/JSONUtils.hpp"

using namespace Leosac;
using namespace Leosac::Auth;

json UserJSONSerializer::serialize(const Auth::User &user, const SecurityContext &sc)
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
    json credentials = {};
    for (const Cred::CredentialLWPtr &cred : user.lazy_credentials())
    {
        SecurityContext::CredentialActionParam cap{.credential_id =
                                                       cred.object_id()};
        if (sc.check_permission(SecurityContext::Action::CREDENTIAL_READ, cap))
        {
            json cred_info = {
                {"id", cred.object_id()},
                {"type",
                 PolymorphicCredentialJSONSerializer::type_name(*cred.load())}};
            credentials.push_back(cred_info);
        }
    }
    // We dont list schedule mapping to websocket client, instead we list
    // schedules.
    std::set<Tools::ScheduleId> schedule_ids;
    json schedules = {};
    for (const Tools::ScheduleMappingLWPtr &mapping : user.lazy_schedules_mapping())
    {
        auto loaded = mapping.load();
        ASSERT_LOG(loaded, "Cannot load. Need to investigate.");
        schedule_ids.insert(loaded->schedule_.object_id());
    }
    for (const auto &id : schedule_ids)
    {
        json sched_info = {{"id", id}, {"type", "schedule"}};
        schedules.push_back(sched_info);
    }

    json serialized = {
        {"id", user.id()},
        {"type", "user"},
        {"attributes",
         {
             {"version", user.odb_version()},
             {"username", user.username()},
             {"firstname", user.firstname()},
             {"lastname", user.lastname()},
             {"rank", static_cast<int>(user.rank())},
             {"validity-enabled", user.validity().is_enabled()},
             {"validity-start", Conversion<std::string>(user.validity().start())},
             {"validity-end", Conversion<std::string>(user.validity().end())},
         }},
        {"relationships",
         {{"memberships", {{"data", memberships}}},
          {"credentials", {{"data", credentials}}},
          {"schedules", {{"data", schedules}}}}}};

    SecurityContext::ActionParam ap;
    ap.user.user_id = user.id();
    if (sc.check_permission(SecurityContext::Action::USER_READ_EMAIL, ap))
    {
        serialized["attributes"]["email"] = user.email();
    }
    return serialized;
}

void UserJSONSerializer::unserialize(Auth::User &out, const json &in,
                                     const SecurityContext &sc)
{
    using namespace Leosac::JSONUtil;

    out.firstname(extract_with_default(in, "firstname", out.firstname()));
    out.lastname(extract_with_default(in, "lastname", out.lastname()));
    out.email(extract_with_default(in, "email", out.email()));
    if (in.find("password") != in.end() && (*in.find("password")).is_string())
    {
        out.password(in.at("password"));
    }

    SecurityContext::ActionParam ap;
    ap.user.user_id = out.id();
    if (sc.check_permission(SecurityContext::Action::USER_UPDATE_RANK, ap))
    {
        // cast to int for json extraction to work, then back to UserRank for
        // setter to work.
        out.rank(static_cast<Auth::UserRank>(
            extract_with_default(in, "rank", static_cast<int>(out.rank()))));
    }
    if (sc.check_permission(SecurityContext::Action::USER_MANAGE_VALIDITY, ap))
    {
        Auth::ValidityInfo validity_default;
        validity_default.set_enabled(out.validity().is_enabled());
        out.validity(
            extract_validity_with_default(in, "validity", validity_default));
    }
}

std::string UserJSONStringSerializer::serialize(const Auth::User &in,
                                                const SecurityContext &sc)
{
    return UserJSONSerializer::serialize(in, sc).dump(4);
}

void UserJSONStringSerializer::unserialize(Auth::User &out, const std::string &in,
                                           const SecurityContext &sc)
{
    json tmp = json::parse(in);
    UserJSONSerializer::unserialize(out, tmp, sc);
}
