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

#pragma once

#include "core/auth/AuthFwd.hpp"
#include "core/credentials/CredentialFwd.hpp"
#include "tools/ToolsFwd.hpp"
#include <vector>

namespace Leosac
{
namespace Tools
{
/**
 * Represent one of the mapping of a schedule.
 *
 * This object is designed to be embedded into Schedule object.
 */
#pragma db object optimistic
struct ScheduleMapping
{
    ScheduleMappingId id() const;

#pragma db id auto
    ScheduleMappingId id_;

#pragma db id_column("schedule_mapping_id") value_column("user_id")
#pragma db value_not_null unordered
    std::vector<Auth::UserLWPtr> users_;

#pragma db id_column("schedule_mapping_id") value_column("group_id")
#pragma db value_not_null unordered
    std::vector<Auth::GroupLWPtr> groups_;

#pragma db id_column("schedule_mapping_id") value_column("credential_id")
#pragma db value_not_null unordered
    std::vector<Cred::CredentialLWPtr> creds_;

    /**
     * A non unique, optional alias.
     */
    std::string alias_;

// todo add door.

#pragma db version
    size_t odb_version_;
};
}
}

#ifdef ODB_COMPILER
#include "core/auth/Group.hpp"
#include "core/auth/User.hpp"
#include "core/credentials/Credential.hpp"
#include "tools/Schedule.hpp"
#endif