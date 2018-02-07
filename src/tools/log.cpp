/*
    Copyright (C) 2014-2016 Leosac

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

#include "log.hpp"
#include "ThreadUtils.hpp"
#include <boost/regex.hpp>

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

    // fixme Maybe update spdlog and use its formatting utilities.
    std::stringstream ss;
    ss << "[" << Leosac::gettid() << "] " << log_msg;
    std::string log_msg_with_thread_id = ss.str();

    switch (level)
    {
    case LogLevel::DEBUG:
        if (console)
            console->debug(log_msg_with_thread_id);
        if (syslog)
            syslog->debug(remove_ascii_format(log_msg_with_thread_id));
        break;
    case LogLevel::INFO:
        if (console)
            console->info(log_msg_with_thread_id);
        if (syslog)
            syslog->info(remove_ascii_format(log_msg_with_thread_id));
        break;
    case LogLevel::WARN:
        if (console)
            console->warn(log_msg_with_thread_id);
        if (syslog)
            syslog->warn(remove_ascii_format(log_msg_with_thread_id));
        break;
    case LogLevel::ERROR:
        if (console)
            console->error(log_msg_with_thread_id);
        if (syslog)
            syslog->error(remove_ascii_format(log_msg_with_thread_id));
        break;
    case LogLevel::CRIT:
        if (console)
            console->critical(log_msg_with_thread_id);
        if (syslog)
            syslog->critical(remove_ascii_format(log_msg_with_thread_id));
        break;
    }
}

LogLevel log_level_from_string(const std::string &level)
{
    if (level == "DEBUG")
        return LogLevel::DEBUG;
    else if (level == "INFO")
        return LogLevel::INFO;
    else if (level == "WARNING")
        return LogLevel::WARN;
    else if (level == "ERROR")
        return LogLevel::ERROR;
    else if (level == "CRIT")
        return LogLevel::CRIT;
    throw std::runtime_error("Invalid log level: " + level);
}
}
