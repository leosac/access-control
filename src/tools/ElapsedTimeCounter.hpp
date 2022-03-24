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

#include <chrono>

namespace Leosac
{

namespace Tools
{

/**
 * This class provide a simple to get the elapsed time since
 * it's creation.
 *
 * The precision of the counter is milliseconds.
 */
class ElapsedTimeCounter
{
  public:
    ElapsedTimeCounter();

    /**
     * Returns the elapsed time, in milliseconds, since the creation of
     * the ElapsedTimeCounter object.
     */
    size_t elapsed() const;

  private:
    using TimePoint = std::chrono::steady_clock::time_point;

    TimePoint creation_;
};
}
}
