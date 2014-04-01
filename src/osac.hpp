/**
 * \file osac.hpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief version handling
 */

#ifndef OSAC_HPP
#define OSAC_HPP

#include <string>

class OSAC
{
public:
    static const int Major = 0;
    static const int Minor = 1;
    static const int Patch = 0;

public:
    /**
     * return OSAC version string formatted this way: MAJOR.MINOR.PATCH
     * @see versionCompare()
     * @return version string
     */
    static std::string& getVersionString();

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
     * compare version string against current OSAC version
     * @see getVersionString()
     * @return 0 on match, -1 if v older than OSAC, 1 if v is newer than OSAC
     */
    static int          versionCompare(const std::string& v);

    /**
     * check validity of a semver version string
     * @see getVersionString()
     * @return true if the version is valid
     */
    static bool         isVersionValid(const std::string& v);

private:
    static const std::string validChars;
};

#endif // OSAC_HPP
