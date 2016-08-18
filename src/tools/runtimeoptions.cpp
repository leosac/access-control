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

/**
 * \file runtimeoptions.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief RuntimeOptions class implementation
 */

#include "runtimeoptions.hpp"

using namespace Leosac::Tools;
const std::string RuntimeOptions::DefaultEmptyParam("undef");

void RuntimeOptions::setFlag(Flag flag, bool value)
{
    _flags.set(flag, value);
}

bool RuntimeOptions::isSet(Flag flag) const
{
    return (_flags.test(flag));
}

void RuntimeOptions::setParam(const std::string &key, const std::string &value)
{
    _params[key] = value;
}

const std::string &RuntimeOptions::getParam(const std::string &key) const
{
    if (_params.count(key) > 0)
        return (_params.at(key));
    else
        throw std::runtime_error("RuntimeOptions::getParam [key = " + key +
                                 "] [value = " + DefaultEmptyParam + "]");
}

bool RuntimeOptions::hasParam(const std::string &key) const
{
    return _params.count(key);
}

bool RuntimeOptions::is_strict() const
{
    return strict_;
}

void RuntimeOptions::set_strict(bool v)
{
    strict_ = v;
}
