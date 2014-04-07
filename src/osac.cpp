/**
 * \file osac.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief version handling
 */

#include "osac.hpp"
#include "tools/version.hpp"

static std::string vstring = std::string();

std::string& OSAC::getVersionString()
{
    if (vstring.empty())
        vstring = Version::buildVersionString(Major, Minor, Patch);
    return (vstring);
}

int OSAC::versionCompare(const std::string& v)
{
    return (Version::versionCompare(v, getVersionString()));
}
