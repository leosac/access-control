/**
 * \file osac.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief version handling
 */

#ifndef OSAC_HPP
#define OSAC_HPP

#include <string>

#ifndef OSAC_VERSION_MAJOR
#define OSAC_VERSION_MAJOR 0
#endif

#ifndef OSAC_VERSION_MINOR
#define OSAC_VERSION_MINOR 0
#endif

#ifndef OSAC_VERSION_PATCH
#define OSAC_VERSION_PATCH 0
#endif

class OSAC
{
    OSAC() = delete;

public:
    static const int Major = OSAC_VERSION_MAJOR;
    static const int Minor = OSAC_VERSION_MINOR;
    static const int Patch = OSAC_VERSION_PATCH;

public:
    /**
     * return OSAC version string formatted this way: MAJOR.MINOR.PATCH
     * @return version string
     * @see versionCompare()
     */
    static std::string& getVersionString();

    /**
     * compare version string against current OSAC version
     * @param v to compare against OSAC
     * @return 0 on match, -1 if v older than OSAC, 1 if v is newer than OSAC
     * @see getVersionString()
     */
    static int          versionCompare(const std::string& v);
};

#endif // OSAC_HPP
