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

#include "AccessPoint.hpp"
#include "Door.hpp"

using namespace Leosac;
using namespace Leosac::Auth;

AccessPoint::AccessPoint()
    : version_(0)
{
}

AccessPointId AccessPoint::id() const
{
    return id_;
}

const std::string &AccessPoint::alias() const
{
    return alias_;
}

void AccessPoint::alias(const std::string &new_alias)
{
    alias_ = new_alias;
}

const std::string &AccessPoint::description() const
{
    return description_;
}

void AccessPoint::description(const std::string &dsc)
{
    description_ = dsc;
}

std::string AccessPoint::controller_module() const
{
    return controller_module_;
}

void AccessPoint::controller_module(const std::string &ctrl_mod)
{
    controller_module_ = ctrl_mod;
}

IDoorPtr AccessPoint::door() const
{
    return door_.lock();
}

DoorId AccessPoint::door_id() const
{
    if (door_.lock())
        return door_.lock()->id();
    return 0;
}
