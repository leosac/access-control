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

#include "core/auth/AuthFwd.hpp"
#include "exception/leosacexception.hpp"
#include "tools/Uuid.hpp"
#include <boost/lexical_cast.hpp>
#include <chrono>
#include <nlohmann/json.hpp>
#include <type_traits>


/**
 * Below we add a serializer into the `nlohmann` namespace to serialize
 * the Leosac::UUID ype. Through ADL our function will be called.
 *
 * This is documented at https://github.com/nlohmann/json#arbitrary-types-conversions
 */

namespace nlohmann
{

template <>
struct adl_serializer<Leosac::UUID>
{
    static void to_json(json &j, const Leosac::UUID &uuid)
    {
        if (uuid.is_nil())
        {
            j = nullptr;
        }
        else
        {
            j = uuid.to_string();
        }
    }

    /**
     * For unserialization we expect either a string representing the
     * UUID, or a number.
     */
    static void from_json(const json &j, Leosac::UUID &uuid)
    {
        if (j.is_null())
        {
            uuid = Leosac::UUID::null_uuid();
        }
        else
        {
            if (j.is_string())
            {
                std::string str = j.get<std::string>();
                uuid = Leosac::UUID(boost::lexical_cast<boost::uuids::uuid>(str));
            }
            else if (j.is_number_unsigned() && j.get<uint64_t>() == 0)
            {
                uuid = Leosac::UUID::null_uuid();
            }
            else
            {
                throw LEOSACException("Failed to unserialize UUID");
            }
        }
    }
};
}

namespace Leosac
{

using json = nlohmann::json;

/**
 * Add a few useful extraction functions.
 *
 * Some of theses are specific to Leosac.
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
typename std::enable_if<!std::is_same<const char *, T>::value &&
                            !std::is_same<std::chrono::system_clock::time_point,
                                          std::remove_reference_t<T>>::value &&
                            !std::is_enum<T>::value,
                        T>::type
extract_with_default(const nlohmann::json &obj, const std::string &key,
                     T default_value)
{
    T ret = default_value;
    try
    {
        if (!obj.at(key).is_null())
            ret = obj.at(key).get<T>();
    }
    catch (const json::out_of_range &e)
    {
    }
    return ret;
}

template <typename T>
typename std::enable_if<std::is_enum<T>::value, T>::type
extract_with_default(const nlohmann::json &obj, const std::string &key,
                     T default_value)
{
    T ret = default_value;
    try
    {
        if (!obj.at(key).is_null())
        {
            // Extract using enum's underlying type, then case back.
            ret = static_cast<T>(obj.at(key).get<std::underlying_type_t<T>>());
        }
    }
    catch (const json::out_of_range &e)
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

/**
 * Extract an ISO 8601 datetime string from a json object.
 * It returns its value as a C++ time_point object.
 */
std::chrono::system_clock::time_point
extract_with_default(const nlohmann::json &obj, const std::string &key,
                     const std::chrono::system_clock::time_point &tp);

/**
 * Extract fields representing a ValidityInfo object.
 *
 * The JSON is expected to look like this:
 *     + `${BASEKEY}-enabled`
 *     + `${BASEKEY}-start`
 *     + `${BASEKEY}-end`
 */
Auth::ValidityInfo extract_validity_with_default(const nlohmann::json &obj,
                                                 const std::string &base_key,
                                                 const Auth::ValidityInfo &def);
}
}
