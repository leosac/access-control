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

#include "AccessPoint.hpp"

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
