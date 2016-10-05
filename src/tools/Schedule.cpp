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

#include "Schedule.hpp"

using namespace Leosac::Tools;

Schedule::Schedule(const std::string &sched_name)
    : name_(sched_name)
    , odb_version_(0)
{
}

bool Schedule::is_in_schedule(const std::chrono::system_clock::time_point &tp) const
{
    for (const auto &tf : timeframes_)
    {
        if (tf.is_in_timeframe(tp))
            return true;
    }
    return false;
}

void Schedule::add_timeframe(const SingleTimeFrame &tf)
{
    timeframes_.push_back(tf);
}

const std::string &Schedule::name() const
{
    return name_;
}

const std::string &Schedule::description() const
{
    return description_;
}

ScheduleId Schedule::id() const
{
    return id_;
}

std::vector<SingleTimeFrame> Schedule::timeframes() const
{
    return timeframes_;
}

size_t Schedule::odb_version() const
{
    return odb_version_;
}

void Schedule::name(const std::string &name)
{
    name_ = name;
}

void Schedule::description(const std::string &desc)
{
    description_ = desc;
}

void Schedule::clear_timeframes()
{
    timeframes_.clear();
}
