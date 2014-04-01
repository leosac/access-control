/**
 * \file osac.cpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief version handling
 */

#include "osac.hpp"

#include <sstream>
#include <algorithm>

const std::string OSAC::validChars = "0123456789.-";

static std::string vstring = std::string();

std::string& OSAC::getVersionString()
{
    if (vstring.empty())
    {
        std::stringstream   ss;

        ss << Major << '.' << Minor << '.' << Patch;
        vstring = ss.str();
    }
    return (vstring);
}

static void cleanVersionString(std::string& v)
{
    v = v.substr(0, v.find_first_of('-'));
    std::replace(v.begin(), v.end(), '.', ' ');
}

int OSAC::versionCompare(std::string a, std::string b)
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

int OSAC::versionCompare(const std::string& v)
{
    return (versionCompare(v, getVersionString()));
}

bool OSAC::isVersionValid(const std::string& v)
{
    return (v.find_first_not_of(validChars) == std::string::npos);
}
