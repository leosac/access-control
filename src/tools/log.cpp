/*
    Copyright (C) 2014-2015 Islog

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

#include <boost/regex.hpp>
#include "log.hpp"

namespace
{
/**
 * Remove the ascii formatting (underline, color, ...) from a string.
 *
 * This is required because writing ascii escape code to log-files
 * is not good.
 */
std::string remove_ascii_format(const std::string &in)
{
    boost::regex regex;
    regex.assign("\033\\[.+m(.*)\033\\[0m");
    std::ostringstream t(std::ios::out | std::ios::binary);
    std::ostream_iterator<char, char> oi(t);
    boost::regex_replace(oi, in.begin(), in.end(), regex, "\\1",
                         boost::match_default | boost::format_sed);
    return t.str();
}
}

namespace LogHelper
{

void log(const std::string &log_msg, int /*line*/, const char * /*funcName*/,
         const char * /*fileName*/, LogLevel level)
{
    auto console = spdlog::get("console");
    auto syslog  = spdlog::get("syslog");
    if (!console && !syslog)
    {
        std::cerr << "Logger not set-up yet ! Will display log message as is."
                  << std::endl;
        std::cerr << log_msg << std::endl;
        return;
    }

    switch (level)
    {
    case LogLevel::DEBUG:
        if (console)
            console->debug(log_msg);
        if (syslog)
            syslog->debug(remove_ascii_format(log_msg));
        break;
    case LogLevel::INFO:
        if (console)
            console->info(log_msg);
        if (syslog)
            syslog->info(remove_ascii_format(log_msg));
        break;
    case LogLevel::NOTICE:
        if (console)
            console->notice(log_msg);
        if (syslog)
            syslog->notice(remove_ascii_format(log_msg));
        break;
    case LogLevel::WARN:
        if (console)
            console->warn(log_msg);
        if (syslog)
            syslog->warn(remove_ascii_format(log_msg));
        break;
    case LogLevel::ERROR:
        if (console)
            console->error(log_msg);
        if (syslog)
            syslog->error(remove_ascii_format(log_msg));
        break;
    case LogLevel::CRIT:
        if (console)
            console->critical(log_msg);
        if (syslog)
            syslog->critical(remove_ascii_format(log_msg));
        break;
    case LogLevel::ALERT:
        if (console)
            console->alert(log_msg);
        if (syslog)
            syslog->alert(remove_ascii_format(log_msg));
        break;
    case LogLevel::EMERG:
        if (console)
            console->emerg(log_msg);
        if (syslog)
            syslog->emerg(remove_ascii_format(log_msg));
        break;
    }
}

LogLevel log_level_from_string(const std::string &level)
{
    if (level == "DEBUG")
        return LogLevel::DEBUG;
    else if (level == "INFO")
        return LogLevel::INFO;
    else if (level == "NOTICE")
        return LogLevel::NOTICE;
    else if (level == "WARNING")
        return LogLevel::WARN;
    else if (level == "ERROR")
        return LogLevel::ERROR;
    else if (level == "ALERT")
        return LogLevel::ALERT;
    else if (level == "EMERGENCY")
        return LogLevel::EMERG;
    throw std::runtime_error("Invalid log level: " + level);
}
}
