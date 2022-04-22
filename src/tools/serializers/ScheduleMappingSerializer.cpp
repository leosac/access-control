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

#include "tools/serializers/ScheduleMappingSerializer.hpp"
#include "core/GetServiceRegistry.hpp"
#include "core/auth/Door_odb.h"
#include "core/auth/User_odb.h"
#include "core/credentials/serializers/PolymorphicCredentialSerializer.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/ScheduleMapping.hpp"
#include "tools/db/DBService.hpp"
#include "tools/registry/ThreadLocalRegistry.hpp"
#include "tools/service/ServiceRegistry.hpp"

using namespace Leosac;
using namespace Leosac::Tools;

json ScheduleMappingJSONSerializer::serialize(const Tools::ScheduleMapping &in,
                                              const SecurityContext &)
{
    // general
    json serialized = {
        {"id", in.id()},
        {"type", "schedule-mapping"},
        {"attributes", {{"alias", in.alias()}, {"version", in.odb_version()}}}};

    json users = json::array();
    for (const auto &user : in.users())
    {
        json json_user = {{"id", user.object_id()}, {"type", "user"}};
        users.push_back(json_user);
    }

    json groups = json::array();
    for (const auto &group : in.groups())
    {
        json json_group = {{"id", group.object_id()}, {"type", "group"}};
        groups.push_back(json_group);
    }

    json creds = json::array();
    for (const auto &cred : in.credentials())
    {
        // Credentials needs to be loaded to determine the underlying type.
        // This is done silently by the serializer.
        json json_cred = {
            {"id", cred.object_id()},
            {"type", PolymorphicCredentialJSONSerializer::type_name(*cred.load())}};
        creds.push_back(json_cred);
    }

    json doors = json::array();
    for (const auto &door : in.doors())
    {
        json json_door = {{"id", door.object_id()}, {"type", "door"}};
        doors.push_back(json_door);
    }

    json zones = json::array();
    for (const auto &zone : in.zones())
    {
        json json_zone = {{"id", zone.object_id()}, {"type", "zone"}};
        zones.push_back(json_zone);
    }

    serialized["relationships"]["users"]       = {{"data", users}};
    serialized["relationships"]["groups"]      = {{"data", groups}};
    serialized["relationships"]["credentials"] = {{"data", creds}};
    serialized["relationships"]["doors"]       = {{"data", doors}};
    serialized["relationships"]["zones"]       = {{"data", zones}};

    return serialized;
}

void ScheduleMappingJSONSerializer::unserialize(Tools::ScheduleMapping &out,
                                                const json &in,
                                                const SecurityContext &)
{
    // We need to database to build Lazy pointer from object's identifier.
    auto db = get_service_registry().get_service<DBService>()->db();
    using namespace JSONUtil;
    out.alias(extract_with_default(in, "alias", out.alias()));

    auto group_ids = in.at("groups");
    out.clear_groups();
    for (const auto &group_id : group_ids)
    {
        auto group = Auth::GroupLPtr(*db, group_id.get<Tools::ScheduleMappingId>());
        out.add_group(group);
    }

    auto user_ids = in.at("users");
    out.clear_users();
    for (const auto &user_id : user_ids)
    {
        Auth::UserLPtr user(*db, user_id.get<Auth::UserId>());
        out.add_user(user);
    }

    auto credential_ids = in.at("credentials");
    out.clear_credential();
    for (const auto &credential_id : credential_ids)
    {
        Cred::CredentialLPtr credential(*db,
                                        credential_id.get<Cred::CredentialId>());
        out.add_credential(credential);
    }

    auto door_ids = in.at("doors");
    out.clear_doors();
    for (const auto &door_id : door_ids)
    {
        Auth::DoorLPtr door(*db, door_id.get<Auth::DoorId>());
        out.add_door(door);
    }

    auto zone_ids = in.at("zones");
    out.clear_zones();
    for (const auto &zone_id : zone_ids)
    {
        Auth::ZoneLPtr zone(*db, zone_id.get<Auth::ZoneId>());
        out.add_zone(zone);
    }
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
