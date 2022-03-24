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

#pragma once

#include "tools/ISchedule.hpp"
#include <boost/property_tree/ptree_fwd.hpp>
#include <map>

namespace Leosac
{
namespace Tools
{
/**
* Load a list of schedules from a boost::property_tree.
*
* The format is defined [here](@ref mod_auth_sched_declare).
* This class was introduced to avoid code duplication.
*/
class XmlScheduleLoader
{
  public:
    /**
    * Load all schedules from a tree and stores them in the map.
    */
    bool load(const boost::property_tree::ptree &t);

    /**
    * Helper function.
    */
    static int week_day_to_int(const std::string &day);

    /**
    * Access the map of stored schedules.
    */
    const std::map<std::string, ISchedulePtr> &schedules() const;

  private:
    /**
    * Adds a new schedule to the map.
    */
    bool extract_one(const boost::property_tree::ptree &node);

    std::map<std::string, ISchedulePtr> schedules_;
};
}
}
