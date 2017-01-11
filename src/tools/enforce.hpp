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

#pragma once

#include "exception/leosacexception.hpp"
#include <utility>

namespace details
{

template <typename E, typename Assessable, typename... Args>
inline auto enforce(Assessable &&value, Args &&... args)
{
    if (!value)
        throw E(std::forward<Args>(args)...);
    return std::forward<Assessable>(value);
}
}

/**
 * Enforce that a condition is true, otherwise
 * throw an exception of type `ex` with parameters
 */
#define ENFORCE(cond, ex_type, ...) ::details::enforce<ex_type>((cond), __VA_ARGS__)

/**
 * Similar to enforce, except that it will throw a LEOSACException.
 */
#define LEOSAC_ENFORCE(cond, ...)                                                   \
    ::details::enforce<LEOSACException>((cond), __VA_ARGS__)
