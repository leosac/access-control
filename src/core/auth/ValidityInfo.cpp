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

#include "core/auth/ValidityInfo.hpp"
#include <cassert>
#include <stdexcept>
#include <tools/log.hpp>

using namespace Leosac::Auth;

ValidityInfo::ValidityInfo()
    : start_(std::chrono::system_clock::time_point::min())
    , end_(std::chrono::system_clock::time_point::max())
    , enabled_(true)
{
}

bool ValidityInfo::is_valid() const
{
    return is_enabled() && is_in_range();
}

bool ValidityInfo::is_enabled() const
{
    return enabled_;
}

bool ValidityInfo::is_in_range() const
{
    TimePoint now = std::chrono::system_clock::now();

    return now >= start_ && now <= end_;
}

void ValidityInfo::set_start_date(const std::string &s)
{
    if (s.empty())
    {
        start_ = std::chrono::system_clock::time_point::min();
        return;
    }

    std::tm tm;
    bzero(&tm, sizeof(tm));
    if (strptime(s.c_str(), "%d/%m/%Y %H:%M", &tm))
    {
        start_ = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    }
    else
    {
        assert(0);
        throw std::runtime_error("invalid date.");
    }
}

void ValidityInfo::set_end_date(const std::string &s)
{
    if (s.empty())
    {
        end_ = std::chrono::system_clock::time_point::max();
        return;
    }

    std::tm tm;
    bzero(&tm, sizeof(tm));
    if (strptime(s.c_str(), "%d/%m/%Y %H:%M", &tm))
    {
        end_ = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    }
    else
    {
        assert(0);
        throw std::runtime_error("invalid date.");
    }
}

void ValidityInfo::set_enabled(bool v)
{
    enabled_ = v;
}

const ValidityInfo::TimePoint &ValidityInfo::start() const
{
    return start_;
}

const ValidityInfo::TimePoint &ValidityInfo::end() const
{
    return end_;
}

void ValidityInfo::start(const ValidityInfo::TimePoint &tp)
{
    start_ = tp;
}

void ValidityInfo::end(const ValidityInfo::TimePoint &tp)
{
    end_ = tp;
}
