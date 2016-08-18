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

#pragma once

#include "database.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <odb/core.hxx>

namespace Leosac
{
namespace Tools
{
/**
 * A log entry.
 *
 * This class is decorated to be ODB friendly. Object of this
 * type will be stored in a sqlite database for logging.
 */
#pragma db object
class LogEntry
{
  public:
#pragma db id auto
    unsigned long id_;

#pragma db not_null
    boost::posix_time::ptime timestamp_;

#pragma db not_null
    std::string msg_;

/**
 * The run_id is generated when Leosac starts, and it is used
 * to identify "runs" (ie if Leosac restarts) in order
 * to better tracks logs.
 */
#pragma db not_null
    std::string run_id_;

#pragma db not_null
    uint8_t level_;

#pragma db not_null
    size_t thread_id_;

  private:
    friend class odb::access;
};

/**
 * SQL view over the LogEntry table.
 */
#pragma db view object(LogEntry)
struct LogView
{
#pragma db column("count(" + LogEntry::id_ + ")")
    size_t count;
};
}
}
