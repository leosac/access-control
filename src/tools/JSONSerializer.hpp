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

#include "tools/log.hpp"
#include <nlohmann/json.hpp>
#include <string>

namespace Leosac
{
using json = nlohmann::json;

/**
 * A JSON serializer that can serialize an object of type `T`.
 */
template <typename T>
struct JSONSerializer
{
    /**
     * Serialize to an std::string
     */
    static std::string to_string(const T &)
    {
        ASSERT_LOG(0, "Reimplement the function.");
        return "";
    }

    /**
     * Serialize to a json object.
     */
    static json to_object(const T &)
    {
        ASSERT_LOG(0, "Reimplement the function.");
        return {};
    }
};
}
