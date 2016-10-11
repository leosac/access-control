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
#include <memory>
#include <odb/lazy-ptr.hxx>

namespace Leosac
{

namespace Tools
{
struct SingleTimeFrame;

class Schedule;
using SchedulePtr   = std::shared_ptr<Schedule>;
using ScheduleId    = unsigned long;
using ScheduleLWPtr = odb::lazy_weak_ptr<Schedule>;

class ISchedule;
using ISchedulePtr  = std::shared_ptr<ISchedule>;
using IScheduleCPtr = std::shared_ptr<const ISchedule>;

class ScheduleMapping;
using ScheduleMappingPtr = std::shared_ptr<ScheduleMapping>;
using ScheduleMappingId  = unsigned long;
}

struct MailInfo;
}
