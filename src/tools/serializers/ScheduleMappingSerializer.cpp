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

#include "tools/serializers/ScheduleMappingSerializer.hpp"
#include "User_odb.h"
#include "core/credentials/serializers/PolymorphicCredentialSerializer.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/ScheduleMapping.hpp"

using namespace Leosac;
using namespace Leosac::Tools;

json ScheduleMappingJSONSerializer::serialize(const Tools::ScheduleMapping &in,
                                              const SecurityContext &sc)
{

    json serialized = {
        {"id", in.id()},
        {"type", "schedule-mapping"},
        {"attributes", {{"alias", in.alias_}, {"version", in.odb_version_}}}};
    json users = json::array();
    for (const auto &user : in.users_)
    {
        json json_user = {{"id", user.object_id()}, {"type", "user"}};
        users.push_back(json_user);
    }
    json groups = json::array();
    for (const auto &group : in.groups_)
    {
        json json_group = {{"id", group.object_id()}, {"type", "group"}};
        groups.push_back(json_group);
    }

    json creds = json::array();
    for (const auto &cred : in.creds_)
    {
        // Credentials needs to be loaded to determine the underlying type.
        // This is done silently by the serializer.
        json json_cred = {
            {"id", cred.object_id()},
            {"type", PolymorphicCredentialJSONSerializer::type_name(*cred.load())}};
        creds.push_back(json_cred);
    }

    serialized["relationships"]["users"]       = {{"data", users}};
    serialized["relationships"]["groups"]      = {{"data", groups}};
    serialized["relationships"]["credentials"] = {{"data", creds}};

    return serialized;
}

void ScheduleMappingJSONSerializer::unserialize(Tools::ScheduleMapping &out,
                                                const json &in,
                                                const SecurityContext &sc)
{
    assert(0);
}

std::string
ScheduleMappingJSONStringSerializer::serialize(const Tools::ScheduleMapping &in,
                                               const SecurityContext &sc)
{
    return ScheduleMappingJSONSerializer::serialize(in, sc).dump(4);
}

void ScheduleMappingJSONStringSerializer::unserialize(Tools::ScheduleMapping &out,
                                                      const std::string &in,
                                                      const SecurityContext &sc)
{
    ScheduleMappingJSONSerializer::unserialize(out, in, sc);
}
