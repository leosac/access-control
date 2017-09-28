/*
    Copyright (C) 2014-2017 Leosac

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
#include "modules/auth/auth-db/AuthDBFwd.hpp"

namespace Leosac
{
namespace Module
{
namespace Auth
{
/**
 * An instance of authentication handler that use the database
 * to verify credentials and permissions.
 */
#pragma db object optimistic
class AuthDBInstance
{
  private:
#pragma db id auto
    AuthDBInstanceId id_;

    /**
     * Target door.
     */
    ::Leosac::Auth::DoorLPtr door_;

#pragma db version
    size_t odb_version_;


    friend odb::access;

    // AUTH_SOURCE device.
    // todo: Hardware service and related infrastructure improvements.
};
}
}
}

#ifdef ODB_COMPILER
#include "core/auth/Door.hpp"
#endif
