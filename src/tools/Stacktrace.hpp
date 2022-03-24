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

#include <string>
#include <vector>

namespace Leosac
{
namespace Tools
{
/**
 * Store information about a stacktrace at a given point in the program
 * execution.
 *
 * The stacktrace represents the states at the time the object
 * was instancied. This object can then be moved around or copied.
 *
 *
 * The reason for this class to be is because `struct backtrace_state *` from
 * GCC's libbacktrace cannot be moved around.
 *
 * @warning This class will be a no-op if its not compiled with GCC.
 */
class Stacktrace
{
  public:
    /**
     * Build a new stacktrace.
     *
     * @param skip The number of stackframe to skip.
     */
    Stacktrace(uint skip /* = 0 */);

    struct Frame
    {
        Frame()
            : suppressed_(false)
            , line_(0)
        {
        }

        /**
         * Flagged as true if the frame represents
         * an unknown function.
         */
        bool suppressed_;
        std::string function_;
        std::string file_;
        int line_;
    };

    std::string str(int max_frames = 10) const;

  private:
    /**
     * We use a vector to represent the stack so we can iterate
     * on the frames.
     */
    std::vector<Frame> stack_;
};
}
}
