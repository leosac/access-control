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

#pragma once

#include "core/auth/AuthFwd.hpp"
#include "core/auth/Interfaces/IAccessProfile.hpp"
#include "core/auth/UserGroupMembership.hpp"
#include "core/auth/ValidityInfo.hpp"
#include "core/credentials/CredentialFwd.hpp"
#include "tools/db/database.hpp"
#include "tools/scrypt/Scrypt.hpp"
#include <memory>

namespace Leosac
{
namespace Auth
{

/**
 * A door.
 */
class IDoor : public std::enable_shared_from_this<IDoor>
{
  public:
    virtual DoorId id() const = 0;

    virtual std::string alias() const       = 0;
    virtual std::string description() const = 0;

    virtual void alias(const std::string &alias)      = 0;
    virtual void description(const std::string &desc) = 0;

    virtual IAccessPointPtr access_point() const = 0;

    virtual AccessPointId access_point_id() const = 0;

    virtual void access_point(IAccessPointPtr) = 0;

    /**
     * Retrieve the lazy pointers to the ScheduleMapping objects that
     * map this door.
     */
    virtual std::vector<Tools::ScheduleMappingLWPtr> lazy_mapping() const = 0;
};
}
}
