/**
 * \file osac.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
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

private:
    static const std::string validChars;
};

#endif // OSAC_HPP
