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
 * \file version.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief version handling
 */

#include "version.hpp"

#include <sstream>
#include <string>
#include <algorithm>

using namespace Leosac::Tools;

const std::string Version::validChars = "0123456789.-";

static std::string vstring = std::string();

std::string Version::buildVersionString(int major, int minor, int patch)
{
    std::ostringstream  oss;

    oss << major << '.' << minor << '.' << patch;
    return (oss.str());
}

static void cleanVersionString(std::string& v)
{
    v = v.substr(0, v.find_first_of('-'));
    std::replace(v.begin(), v.end(), '.', ' ');
}

int Version::versionCompare(std::string a, std::string b)
{
    std::istringstream  issa;
    std::istringstream  issb;
    int                 va;
    int                 vb;

    cleanVersionString(a);
    cleanVersionString(b);
    issa.str(a);
    issb.str(b);
    for (int i = 0; i < 3; ++i)
    {
        issa >> va;
        issb >> vb;
        if (va > vb)
            return (1);
        else if (va < vb)
            return (-1);
    }
    return (0);
}

bool Version::isVersionValid(const std::string& v)
{
    if (v.find_first_not_of(validChars) != std::string::npos)
        return (false);
    if (v.empty())
        return (false);
    if (v.front() == '.' || v.back() == '.')
        return (false);
    if (std::count_if(v.begin(), v.end(), [] (char a) { return (a == '.');} ) != 2)
        return (false);
    if (v.find_first_of('.') + 1 == v.find_last_of('.'))
        return (false);
    return (true);
}
