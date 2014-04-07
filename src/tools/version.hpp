/**
 * \file version.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief version handling
 */

#ifndef VERSION_HPP
#define VERSION_HPP

#include <string>

class Version
{
public:
    /**
     * return semver compatible version string formatted this way: MAJOR.MINOR.PATCH
     * @see getVersionString()
     * @return version string
     */
    static std::string  buildVersionString(int major, int minor, int patch);

    /**
     * compare two version strings using semver v2.0.0
     * @see getVersionString()
     * @return 0 on match, -1 if a older than b, 1 if a is newer than b
     */
    static int          versionCompare(std::string a, std::string b);

    /**
     * check validity of a semver version string
     * @see getVersionString()
     * @return true if the version is valid
     */
    static bool         isVersionValid(const std::string& v);

private:
    static const std::string validChars;
};

#endif // VERSION_HPP
