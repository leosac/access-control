/*
    Copyright (C) 2014-2015 Islog

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

/**
 * \file leosac.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief version and platform handling
 */

#include "leosac.hpp"
#include "tools/version.hpp"

using namespace Leosac::Tools;
static std::string vstring = std::string();

std::string& Leosac::getVersionString()
{
    if (vstring.empty())
        vstring = Version::buildVersionString(Major, Minor, Patch);
    return (vstring);
}

int Leosac::versionCompare(const std::string& v)
{
    return (Version::versionCompare(v, getVersionString()));
}
