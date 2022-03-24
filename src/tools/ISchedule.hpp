/*
    Copyright (C) 2014-2022 Leosac

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

#include "tools/ToolsFwd.hpp"
#include <chrono>
#include <string>
#include <vector>

#pragma once

namespace Leosac
{
namespace Tools
{
/*
 * The interface for Schedule object.
 *
 * This class is not "ODB aware". Its main purpose
 * is to prevent too many includes for clients that do not need
 * to interact with the database.
 */
class ISchedule : public std::enable_shared_from_this<ISchedule>
{
  public:
    virtual ~ISchedule() = default;

    virtual ScheduleId id() const = 0;

    /**
    * Retrieve the name of the schedule.
    */
    virtual const std::string &name() const = 0;

    virtual void name(const std::string &) = 0;

    virtual const std::string &description() const = 0;

    virtual void description(const std::string &) = 0;

    /**
    * Check whether or not the given time point can be found in the schedule.
    */
    virtual bool
    is_in_schedule(const std::chrono::system_clock::time_point &tp) const = 0;

    /**
    * Add the given timeframe to this schedule;
    */
    virtual void add_timeframe(const SingleTimeFrame &tf) = 0;

    /**
     * Remove all the timeframes from this schedule.
     */
    virtual void clear_timeframes() = 0;

    /**
     * Retrieves the list of timesframes that compose
     * this schedule.
     */
    virtual std::vector<SingleTimeFrame> timeframes() const = 0;

    virtual void add_mapping(const Tools::ScheduleMappingPtr &map) = 0;

    virtual void clear_mapping() = 0;

    virtual std::vector<Tools::ScheduleMappingPtr> mapping() const = 0;


    virtual size_t odb_version() const = 0;
};
}
}
