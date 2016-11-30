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

#include "core/auth/Door.hpp"
#include "core/auth/AccessPoint.hpp"
#include "tools/AssertCast.hpp"

using namespace Leosac::Auth;

Door::Door()
    : version_(0)
{
}

DoorId Door::id() const
{
    return id_;
}

std::string Door::alias() const
{
    return alias_;
}

std::string Door::description() const
{
    return desc_;
}

void Door::alias(const std::string &alias)
{
    alias_ = alias;
}

void Door::description(const std::string &desc)
{
    desc_ = desc;
}

IAccessPointPtr Door::access_point() const
{
    return access_point_;
}

void Door::access_point(IAccessPointPtr ptr)
{
    access_point_ = assert_cast<AccessPointPtr>(ptr);
    if (access_point_)
        access_point_->door_ = assert_cast<DoorPtr>(shared_from_this());
}

std::vector<Leosac::Tools::ScheduleMappingLWPtr> Door::lazy_mapping() const
{
    return schedules_mapping_;
}

void Door::schedule_mapping_added(
    const Leosac::Tools::ScheduleMappingPtr &sched_mapping)
{
    schedules_mapping_.push_back(sched_mapping);
}

AccessPointId Door::access_point_id() const
{
    if (access_point_)
        return access_point_->id();
    return 0;
}
