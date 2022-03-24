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

#include <boost/signals2.hpp>

namespace Leosac
{
/**
 * A shortname for the boost::signals2 namespace.
 */
namespace bs2 = boost::signals2;

/**
 * A boost::signals2 combiner that appends vector together.
 */
template <typename T>
struct VectorAppenderCombiner
{
    using result_type = std::vector<T>;

    template <typename InputIterator>
    std::vector<T> operator()(InputIterator first, InputIterator last) const
    {
        std::vector<T> result;
        if (first == last)
            return {};

        while (first != last)
        {
            std::vector<T> this_slot_result = *first;
            result.insert(result.end(), this_slot_result.begin(),
                          this_slot_result.end());
            ++first;
        }
        return result;
    }
};

/**
 * A boost::signals2 combiner that makes sure that at most
 * one slot returns a non-null pointer.
 *
 * This combiner works for pointer object.
 */
template <typename T>
struct AtMostOneCombiner
{
    using result_type = T;

    template <typename InputIterator>
    T operator()(InputIterator first, InputIterator last) const
    {
        bool set = false;
        T result = nullptr;
        if (first == last)
            return T{};

        while (first != last)
        {
            T tmp = nullptr;
            tmp   = *first;

            if (tmp && set)
            {
                throw LEOSACException("AtMostOneCombiner: A non-null pointer has "
                                      "already been returned.");
            }
            else if (tmp)
            {
                result = tmp;
                set    = true;
            }

            ++first;
        }
        return result;
    }
};
}
