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

/**
* \file version.hpp
* \brief version handling
*/

#ifndef VERSION_HPP
#define VERSION_HPP

#include <string>

namespace Leosac
{
namespace Tools
{
class Version
{
    static const int Major = LEOSAC_VERSION_MAJOR;
    static const int Minor = LEOSAC_VERSION_MINOR;
    static const int Patch = LEOSAC_VERSION_PATCH;

    Version() = delete;

  public:
    /**
    * return semver compatible version string formatted this way: MAJOR.MINOR.PATCH
    * @see getVersionString()
    * @return version string
    */
    static std::string buildVersionString(int major, int minor, int patch,
                                          std::string git_sha1 = "");

    /**
    * compare two version strings using semver v2.0.0
    * @see getVersionString()
    * @return 0 on match, -1 if a older than b, 1 if a is newer than b
    */
    static int versionCompare(std::string a, std::string b);

    /**
    * check validity of a semver version string
    * @see getVersionString()
    * @return true if the version is valid
    */
    static bool isVersionValid(const std::string &v);


    /**
     * Retrieve the short (X.Y.Z) version of Leosac.
     */
    static std::string get_short_version();

    /**
     * Returns the complete version string.
     *
     * Something like: X.Y.Z-GIT_SHA1
     */
    static std::string get_full_version();

  private:
    static const std::string validChars;
};
}
}

#endif // VERSION_HPP
