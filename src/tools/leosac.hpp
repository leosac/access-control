/**
 * \file leosac.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief version handling
 */

#ifndef LEOSAC_HPP
#define LEOSAC_HPP

#include <string>

#ifndef LEOSAC_VERSION_MAJOR
#   define LEOSAC_VERSION_MAJOR 0
#endif

#ifndef LEOSAC_VERSION_MINOR
#   define LEOSAC_VERSION_MINOR 0
#endif

#ifndef LEOSAC_VERSION_PATCH
#   define LEOSAC_VERSION_PATCH 0
#endif

#if defined(LEOSAC_PLATFORM_RASPBERRYPI)
#   define LEOSAC_PLATFORM  (PlatformType::RaspberryPi)
#elif defined(LEOSAC_PLATFORM_CUBIEBOARD)
#   define LEOSAC_PLATFORM  (PlatformType::CubieBoard)
#else
#   define LEOSAC_PLATFORM  (PlatformType::None)
#endif

namespace Leosac
{
    static const int Major = LEOSAC_VERSION_MAJOR;
    static const int Minor = LEOSAC_VERSION_MINOR;
    static const int Patch = LEOSAC_VERSION_PATCH;

    enum class PlatformType {
        None = 0,
        RaspberryPi,
        CubieBoard
    };
    static const PlatformType Platform = LEOSAC_PLATFORM;

    /**
     * return LEOSAC version string formatted this way: MAJOR.MINOR.PATCH
     * @return version string
     * @see versionCompare()
     */
    std::string&    getVersionString();

    /**
     * compare version string against current LEOSAC version
     * @param v to compare against LEOSAC
     * @return 0 on match, -1 if v older than LEOSAC, 1 if v is newer than LEOSAC
     * @see getVersionString()
     */
    int             versionCompare(const std::string& v);
};

#undef LEOSAC_VERSION_MAJOR
#undef LEOSAC_VERSION_MINOR
#undef LEOSAC_VERSION_PATCH
#undef LEOSAC_PLATFORM

#endif // LEOSAC_HPP
