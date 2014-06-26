/**
 * \file leosac.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief version handling
 */

#ifndef LEOSAC_HPP
#define LEOSAC_HPP

#include <string>

#ifndef LEOSAC_VERSION_MAJOR
#define LEOSAC_VERSION_MAJOR 0
#endif

#ifndef LEOSAC_VERSION_MINOR
#define LEOSAC_VERSION_MINOR 0
#endif

#ifndef LEOSAC_VERSION_PATCH
#define LEOSAC_VERSION_PATCH 0
#endif

class LEOSAC
{
    LEOSAC() = delete;

public:
    static const int Major = LEOSAC_VERSION_MAJOR;
    static const int Minor = LEOSAC_VERSION_MINOR;
    static const int Patch = LEOSAC_VERSION_PATCH;

public:
    /**
     * return LEOSAC version string formatted this way: MAJOR.MINOR.PATCH
     * @return version string
     * @see versionCompare()
     */
    static std::string& getVersionString();

    /**
     * compare version string against current LEOSAC version
     * @param v to compare against LEOSAC
     * @return 0 on match, -1 if v older than LEOSAC, 1 if v is newer than LEOSAC
     * @see getVersionString()
     */
    static int          versionCompare(const std::string& v);
};

#endif // LEOSAC_HPP
