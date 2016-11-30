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

#include "Stacktrace.hpp"
#include "Colorize.hpp"
#include "log.hpp"
#include <backtrace.h>
#include <boost/core/demangle.hpp>
#include <boost/range/adaptor/reversed.hpp>

using namespace Leosac;
using namespace Leosac::Tools;

Stacktrace::Stacktrace(uint skip /* = 0 */)
{
    auto cb_error = [](void * /*data*/, const char *msg, int errnum) {
        ERROR("Problem when generating stacktrace: " << msg
                                                     << ". Error code: " << errnum);
    };

    auto cb_full = [](void *data, uintptr_t /*pc*/, const char *filename, int line,
                      const char *function) -> int {
        std::vector<Frame> *stack = static_cast<std::vector<Frame> *>(data);
        Frame frame;
        if (!function)
        {
            frame.suppressed_ = true;
        }
        else
        {
            frame.function_ = boost::core::demangle(function);
            frame.line_     = line;
            frame.file_     = filename;
        }
        stack->push_back(frame);
        return 0;
    };

    struct backtrace_state *bt =
        backtrace_create_state(nullptr, 0, nullptr, nullptr);
    backtrace_full(bt, skip, cb_full, cb_error, &stack_);
}

std::string Stacktrace::str(int max_frames /* = 10 */) const
{
    int frame_count      = 0;
    int suppressed_count = 0; // count suppressed frame that follows each other.
    std::stringstream ss;

    for (const auto &frame : stack_)
    {
        if (frame_count == max_frames)
            break;
        if (frame.suppressed_)
        {
            suppressed_count++;
            continue;
        }
        if (suppressed_count)
            ss << Colorize::red("Frames suppressed: ") << suppressed_count
               << std::endl;
        suppressed_count = 0;

        ss << "#" << frame_count << " in " << Colorize::cyan(frame.function_)
           << " at " << Colorize::bright_green(frame.file_) << ":" << frame.line_
           << std::endl;
        frame_count++;
    }
    return ss.str();
}
