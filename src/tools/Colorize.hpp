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

#pragma once

#include <string>
#include <sstream>

namespace Leosac
{
  /**
   * A collection of function to colorize / format text.
   */
  namespace Colorize
  {
    /**
     * Implementation namespace.
     *
     * Do not use.
     */
    namespace detail
    {
      static std::string clear()
      {
        return "\033[0m";
      }

      /**
       * Return a string containing the escape code, a string representation
       * of T and the clear escape string.
       */
      template <typename T>
      std::string format(const std::string &escape_code, const T &in)
      {
        std::stringstream ss;
        ss << "\033[" << escape_code << "m" << in << clear();
        return ss.str();
      }
    }

    template <typename T>
    std::string bold(const T &in)
    {
      return detail::format("1", in);
    }

    template <typename T>
    std::string underline(const T &in)
    {
      return detail::format("4", in);
    }
    template <typename T>
    std::string red(const T &in)
    {
      return detail::format("31", in);
    }

    template <typename T>
    std::string green(const T &in)
    {
      return detail::format("32", in);
    }
  }
}
