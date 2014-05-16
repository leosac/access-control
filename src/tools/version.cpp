/**
 * \file version.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief version handling
 */

#include "version.hpp"

#include <sstream>
#include <string>
#include <algorithm>

const std::string Version::validChars = "0123456789.-";

static std::string vstring = std::string();

std::string Version::buildVersionString(int major, int minor, int patch)
{
    std::stringstream   ss;

    ss << major << '.' << minor << '.' << patch;
    return (ss.str());
}

static void cleanVersionString(std::string& v)
{
    v = v.substr(0, v.find_first_of('-'));
    std::replace(v.begin(), v.end(), '.', ' ');
}

int Version::versionCompare(std::string a, std::string b)
{
    std::stringstream   ssa;
    std::stringstream   ssb;
    int                 va;
    int                 vb;

    cleanVersionString(a);
    cleanVersionString(b);
    ssa.str(a);
    ssb.str(b);
    for (int i = 0; i < 3; ++i)
    {
        ssa >> va;
        ssb >> vb;
        if (va > vb)
            return (1);
        else if (va < vb)
            return (-1);
    }
    return (0);
}

bool Version::isVersionValid(const std::string& v)
{
    return (v.find_first_not_of(validChars) == std::string::npos);
}
