/**
 * \file leosac.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief version and platform handling
 */

#include "leosac.hpp"
#include "tools/version.hpp"

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
