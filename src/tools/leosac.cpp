/**
 * \file leosac.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief version handling
 */

#include "leosac.hpp"
#include "tools/version.hpp"

static std::string vstring = std::string();

std::string& LEOSAC::getVersionString()
{
    if (vstring.empty())
        vstring = Version::buildVersionString(Major, Minor, Patch);
    return (vstring);
}

int LEOSAC::versionCompare(const std::string& v)
{
    return (Version::versionCompare(v, getVersionString()));
}
