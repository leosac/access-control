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

#pragma once
#include <json.hpp>

namespace Leosac
{
/**
 * Add a few useful extraction functions.
 */
namespace JSONUtil
{
/**
 * Extract the value of a key from a json object.
 *
 * If the key cannot be found, or if it is null, this function returns the default
 * value instead.
 */
template <typename T>
typename std::enable_if<!std::is_same<const char *, T>::value, T>::type
extract_with_default(const nlohmann::json &obj, const std::string &key,
                     T default_value)
{
    T ret = default_value;
    try
    {
        if (!obj.at(key).is_null())
            ret = obj.at(key).get<T>();
    }
    catch (const std::out_of_range &e)
    {
    }
    return ret;
}

template <typename T>
typename std::enable_if<std::is_same<const char *, T>::value, std::string>::type
extract_with_default(const nlohmann::json &obj, const std::string &key,
                     T default_value)
{
    return extract_with_default<std::string>(obj, key, default_value);
}
}
}
