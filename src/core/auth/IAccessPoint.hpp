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
#include <string>

namespace Leosac
{
namespace Auth
{
/**
 * An interface for access point.
 *
 * An access point can be thought of as a point where access control
 * is performed.
 */
class IAccessPoint
{
  public:
    virtual AccessPointId id() const = 0;

    virtual const std::string &alias() const         = 0;
    virtual void alias(const std::string &new_alias) = 0;

    virtual const std::string &description() const   = 0;
    virtual void description(const std::string &dsc) = 0;
};
}
}
