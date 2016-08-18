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

#include "SingleTimeFrame.hpp"
#include <chrono>
#include <vector>
#include <string>

#pragma once

namespace Leosac
{
namespace Tools
{
/**
* A schedule is simply a list of time frame (SingleTimeFrame) with
* a name.
*/
class Schedule
{
  public:
    Schedule(const std::string &sched_name = "");

    /**
    * Retrieve the name of the schedule.
    */
    const std::string &name() const;

    /**
    * Check whether or not the given time point can be found in the schedule.
    */
    bool is_in_schedule(const std::chrono::system_clock::time_point &tp) const;

    /**
    * Add the given timeframe to this schedule;
    */
    void add_timeframe(const SingleTimeFrame &tf);

  private:
    std::vector<SingleTimeFrame> timeframes_;
    std::string name_;
};
}
}
