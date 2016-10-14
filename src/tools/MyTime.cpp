/*
    Copyright (C) 2014-2016 Islog

    This file is part of Leosac.

    Leosac is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leosac is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "tools/MyTime.hpp"
#include "exception/leosacexception.hpp"
#include "tools/log.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace Leosac
{
time_t my_timegm(struct tm *tm)
{
    time_t ret;
    char *tz;

    tz = getenv("TZ");
    if (tz)
        tz = strdup(tz);
    setenv("TZ", "", 1);
    tzset();
    ret = mktime(tm);
    if (tz)
    {
        setenv("TZ", tz, 1);
        free(tz);
    }
    else
        unsetenv("TZ");
    tzset();
    return ret;
}

bool my_gettime(std::tm *out, const std::string &date_str, const char *fmt)
{
    /*
        std::istringstream iss(date_str);
        iss >> std::get_time(out, "%Y-%m-%dT%H:%M:%SZ");
        if (!iss.good())
            throw ::LEOSACException("Failed to parse date.");

        return true;
    */
    char *ret = strptime(date_str.c_str(), fmt, out);
    if (ret != date_str.c_str() + date_str.length())
        return false;
    return true;
}

bool my_puttime(std::string &out, const std::tm *tt, const char *fmt)
{ /*
     std::stringstream ss;
     ss << std::put_time(tt, fmt);
     out = ss.str();
     return true;*/

    out.resize(150);
    if (size_t sz = strftime(&out[0], 150, fmt, tt))
    {
        out.resize(sz);
        return true;
    }
    return false;
}

std::string to_local_rfc2822(const std::chrono::system_clock::time_point &tp)
{
    std::string out;
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);

    bool ret = my_puttime(out, std::localtime(&tt), "%a, %d %b %Y %T %z");
    ASSERT_LOG(ret, "Failed to convert date to RFC2822");

    return out;
}
}
