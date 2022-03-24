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

#include "exception/leosacexception.hpp"
#include <spdlog/fmt/fmt.h>

namespace Leosac
{

/**
 * A class to represents invalid argument exception
 * in Leosac.
 *
 * This class has the same meaning as std::invalid_argument
 * but inherits from LEOSACException so it can carry additional
 * information.
 *
 * Check the constructor documentation for more.
 *
 * @note Generally, this exception class can be automatically
 * thrown using the LEOSAC_ENFORCE_ARGUMENT macro.
 */
class InvalidArgument : public LEOSACException
{
  public:
    /**
     * The constructor takes multiple parameter in order to
     * output an informative message.
     *
     * @param arg_name: Which variable was invalid.
     * @param arg_val: The value of that variable.
     * @param what: An human friendly description of the expectations.
     * @param cond_str: The string representation of the condition.
     */
    template <typename T>
    InvalidArgument(const std::string &arg_name, const T &arg_val,
                    const std::string &what, const std::string &cond_str)
        : LEOSACException("")
    {
        message_ = fmt::format(
            "Invalid Argument {}. Value was {}. Condition was {}. Friendly {}",
            arg_name, arg_val, cond_str, what);
    }
};
}
