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

#include <exception>
#include <iostream>
#include <tools/log.hpp>
#include "ExceptionsTools.hpp"

void Leosac::print_exception(const std::exception &e, int level /* = 0 */)
{
    std::cerr << std::string(level, ' ') << "exception: " << e.what() << '\n';
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
        std::cerr << "unkown exception type" << std::endl;
    }
}

void Leosac::log_exception(const std::exception &e, int level /* = 0 */)
{
    ERROR(std::string(level, ' ') << "exception: " << e.what());
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
        ERROR("unkown exception type");
    }
}

