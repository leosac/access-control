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

#include "core/credentials/serializers/CredentialSerializer.hpp"
#include "Schedule_odb.h"
#include "User_odb.h"
#include "core/GetServiceRegistry.hpp"
#include "core/SecurityContext.hpp"
#include "core/auth/ValidityInfo.hpp"
#include "core/credentials/ICredential.hpp"
#include "tools/Conversion.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/registry/ThreadLocalRegistry.hpp"

using namespace Leosac;
using namespace Leosac::Cred;

json CredentialJSONSerializer::serialize(const Cred::ICredential &in,
                                         const SecurityContext &)
{
    std::set<Tools::ScheduleId> schedule_ids;
    json schedules = {};
    for (const Tools::ScheduleMappingLWPtr &mapping : in.lazy_schedules_mapping())
    {
        auto loaded = mapping.load();
        ASSERT_LOG(loaded, "Cannot load. Need to investigate.");
        schedule_ids.insert(loaded->schedule_id());
    }
    for (const auto &id : schedule_ids)
    {
        json sched_info = {{"id", id}, {"type", "schedule"}};
        schedules.push_back(sched_info);
    }

    json serialized = {
        {"id", in.id()},
        {"type", "credential"},
        {"attributes",
         {
             {"version", in.odb_version()},
             {"alias", in.alias()},
             {"description", in.description()},
             {"validity-enabled", in.validity().is_enabled()},
             {"validity-start", Conversion<std::string>(in.validity().start())},
             {"validity-end", Conversion<std::string>(in.validity().end())},
         }}};

    if (in.owner_id())
    {
        serialized["relationships"]["owner"] = {
            {"data", {{"id", in.owner_id()}, {"type", "user"}}}};
    }
    serialized["relationships"]["schedules"] = {{"data", schedules}};
    return serialized;
}

void CredentialJSONSerializer::unserialize(Cred::ICredential &out, const json &in,
                                           const SecurityContext &)
{
    using namespace JSONUtil;
    out.alias(extract_with_default(in, "alias", out.alias()));
    out.description(extract_with_default(in, "description", out.description()));

    // Credential validity attributes
    Auth::ValidityInfo validity_default;
    validity_default.set_enabled(out.validity().is_enabled());
    out.validity(extract_validity_with_default(in, "validity", validity_default));

    // Owner
    Auth::UserId new_owner_id = extract_with_default(in, "owner_id", out.owner_id());
    if (new_owner_id != out.owner_id())
    {
        if (new_owner_id)
        {
            DBPtr dbptr = get_service_registry().get_service<DBService>()->db();
            Auth::UserLPtr new_owner(*dbptr, new_owner_id);
            out.owner(new_owner);
        }
        else
        {
            out.owner(nullptr);
        }
    }
}
