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

#pragma once

#include "core/auth/AuthTarget.hpp"
#include <chrono>
#include <memory>

namespace Leosac
{
namespace Auth
{
class IAccessProfile;
using IAccessProfilePtr = std::shared_ptr<IAccessProfile>;

/**
* Holds information about access permission
*/
class IAccessProfile
{
  public:
    /**
    * Does the profile allow access to the user.
    *
    * @param date the time point at which we try to get access.
    * @param target the door we want to open.
    *
    * @note Check for defaults permissions then door-specific permissions.
    */
    virtual bool isAccessGranted(const std::chrono::system_clock::time_point &date,
                                 AuthTargetPtr target) = 0;

    virtual size_t schedule_count() const = 0;
};
}
}
