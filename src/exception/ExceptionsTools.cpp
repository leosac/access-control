/*
    Copyright (C) 2014-2016 Islog

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

#include "ExceptionsTools.hpp"
#include "tools/Colorize.hpp"
#include "tools/log.hpp"

namespace
{
/**
 * Create a string to prepend to the exception message, for
 * pretty printing.
 */
std::string build_prepend_identation(int level)
{
    std::string prepend;
    if (level)
        prepend = '|';
    for (int i = 0; i < level; ++i)
        prepend += "--";
    if (level)
        prepend += "> ";

    return prepend;
}
}

void Leosac::print_exception(const std::exception &e, int level /* = 0 */)
{
    std::cerr << build_prepend_identation(level) << "exception: " << e.what()
              << '\n';
    try
    {
        std::rethrow_if_nested(e);
    }
    catch (const std::exception &e2)
    {
        print_exception(e2, level + 1);
    }
    catch (...)
    {
        std::cerr << "Unknown exception type." << std::endl;
    }
}

void Leosac::log_exception(const std::exception &e, int level /* = 0 */)
{
    ERROR(build_prepend_identation(level) << "exception: " << e.what());
    try
    {
        std::rethrow_if_nested(e);
    }
    catch (const std::exception &e2)
    {
        log_exception(e2, level + 1);
    }
    catch (...)
    {
        ERROR("Unknown exception type.");
    }
}
