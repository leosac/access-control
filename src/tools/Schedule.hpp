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

#pragma once

#include "LeosacFwd.hpp"
#include "tools/ISchedule.hpp"
#include "tools/ScheduleMapping.hpp"
#include "tools/SingleTimeFrame.hpp"
#include "tools/ToolsFwd.hpp"
#include "tools/db/database.hpp"
#include <chrono>
#include <string>
#include <vector>

namespace Leosac
{
namespace Tools
{
/**
* A schedule is simply a list of time frame (SingleTimeFrame) with
* a name.
*/
#pragma db object optimistic
class Schedule : public virtual ISchedule
{
  public:
    Schedule(const std::string &sched_name = "");
    virtual ~Schedule()                    = default;
    Schedule(const Schedule &)             = default;

    const std::string &name() const override;

    bool
    is_in_schedule(const std::chrono::system_clock::time_point &tp) const override;

    void add_timeframe(const SingleTimeFrame &tf) override;

    const std::string &description() const override;

    void name(const std::string &) override;

    void description(const std::string &) override;

    void clear_timeframes() override;

    ScheduleId id() const override;

    std::vector<SingleTimeFrame> timeframes() const override;

    void add_mapping(const Tools::ScheduleMappingPtr &map) override;

    void clear_mapping() override;

    std::vector<ScheduleMappingPtr> mapping() const override;

    size_t odb_version() const override;

  private:
    friend class odb::access;
    friend class ::Leosac::TestAccess;

#pragma db id auto
    ScheduleId id_;

#pragma db id_column("schedule_id") value_column("timeframe")
    std::vector<SingleTimeFrame> timeframes_;

#pragma db unique
    std::string name_;

    std::string description_;

#pragma db id_column("schedule_id") value_column("schedule_mapping_id")
    std::vector<Tools::ScheduleMappingPtr> mapping_;

#pragma db version
    size_t odb_version_;
};

class ScheduleValidator
{
  public:
    static void validate(const ISchedule &sched);
    static void validate_name(const std::string &name);
};
}
}

#ifdef ODB_COMPILER
#include "tools/ScheduleMapping.hpp"
#endif
