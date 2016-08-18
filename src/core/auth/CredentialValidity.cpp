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

#include <stdexcept>
#include <cassert>
#include <tools/log.hpp>
#include "core/auth/CredentialValidity.hpp"

using namespace Leosac::Auth;

CredentialValidity::CredentialValidity()
    : validity_start_(std::chrono::system_clock::time_point::min())
    , validity_end_(std::chrono::system_clock::time_point::max())
    , enabled_(true)
{
}

bool CredentialValidity::is_valid() const
{
    return is_enabled() && is_in_range();
}

bool CredentialValidity::is_enabled() const
{
    return enabled_;
}

bool CredentialValidity::is_in_range() const
{
    TimePoint now = std::chrono::system_clock::now();

    if (now >= validity_start_ && now <= validity_end_)
        return true;
    return false;
}

void CredentialValidity::set_start_date(const std::string &s)
{
    if (s.empty())
    {
        validity_start_ = std::chrono::system_clock::time_point::min();
        return;
    }

    std::tm tm;
    bzero(&tm, sizeof(tm));
    if (strptime(s.c_str(), "%d/%m/%Y %H:%M", &tm))
    {
        validity_start_ = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    }
    else
    {
        assert(0);
        throw std::runtime_error("invalid date.");
    }
}

void CredentialValidity::set_end_date(const std::string &s)
{
    if (s.empty())
    {
        validity_end_ = std::chrono::system_clock::time_point::max();
        return;
    }

    std::tm tm;
    bzero(&tm, sizeof(tm));
    if (strptime(s.c_str(), "%d/%m/%Y %H:%M", &tm))
    {
        validity_end_ = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    }
    else
    {
        assert(0);
        throw std::runtime_error("invalid date.");
    }
}

void CredentialValidity::set_enabled(bool v)
{
    enabled_ = v;
}
