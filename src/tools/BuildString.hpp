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


template<typename T>
static void add_one(const T & arg, std::stringstream &ss)
{
    ss << arg;
}

static void print_more(std::stringstream &)
{
}

template<typename T, typename ...Args>
static void print_more(std::stringstream &ss, const T&arg, Args ...args)
{
    add_one(arg, ss);
    print_more(ss, args...);
}

/**
 * Use a stringstream to build a string that combine all arguments.
 */
template<typename ...Args>
std::string build_str(Args... args)
{
    std::stringstream ss;
    print_more(ss, args...);

    return ss.str();
}