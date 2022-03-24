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

#include "log.hpp"
#include <chrono>

namespace Leosac
{
namespace Tools
{
/**
 * Compute the time until the next timeout from a collection of time point.
 *
 * This function will iterate over a collection of time point and return
 * the number of milliseconds until the soonest timeout.
 *
 * If the collection is empty or if all the time point are time_point::max()
 * the function return -1.
 */
template <class InputIterator>
int compute_timeout(InputIterator begin, InputIterator end)
{
    static_assert(
        std::is_same<
            std::chrono::system_clock::time_point,
            typename std::iterator_traits<InputIterator>::value_type>::value,
        "Iterator dereference to wrong type");
    auto tp = std::chrono::system_clock::time_point::max();

    while (begin != end)
    {
        if (*begin < tp)
            tp = *begin;
        ++begin;
    }

    if (tp == std::chrono::system_clock::time_point::max())
        return -1; // no update asked.

    int timeout = std::chrono::duration_cast<std::chrono::milliseconds>(
                      tp - std::chrono::system_clock::now())
                      .count();
    DEBUG("Next timeout: " << timeout);
    return timeout < 0 ? 0 : timeout;
}
}
}
