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

/**
 * \file leosac.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief version and platform handling
 */

#ifndef LEOSAC_HPP
#define LEOSAC_HPP

#include <string>

namespace Leosac
{
static const int Major = LEOSAC_VERSION_MAJOR;
static const int Minor = LEOSAC_VERSION_MINOR;
static const int Patch = LEOSAC_VERSION_PATCH;

/**
 * return LEOSAC version string formatted this way: MAJOR.MINOR.PATCH
 * @return version string
 * @see versionCompare()
 */
std::string &getVersionString();

/**
 * compare version string against current LEOSAC version
 * @param v to compare against LEOSAC
 * @return 0 on match, -1 if v older than LEOSAC, 1 if v is newer than LEOSAC
 * @see getVersionString()
 */
int versionCompare(const std::string &v);
};

#endif // LEOSAC_HPP
