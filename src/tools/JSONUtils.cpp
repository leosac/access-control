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

#include "tools/JSONUtils.hpp"
#include "core/auth/ValidityInfo.hpp"
#include "tools/MyTime.hpp"
#include <date/date.h>

namespace Leosac
{
namespace JSONUtil
{
Auth::ValidityInfo extract_validity_with_default(const nlohmann::json &obj,
                                                 const std::string &base_key,
                                                 const Auth::ValidityInfo &def)
{
    auto validity = def;

    validity.set_enabled(
        extract_with_default(obj, base_key + "-enabled", def.is_enabled()));

    validity.start(extract_with_default(obj, base_key + "-start", def.start()));
    validity.end(extract_with_default(obj, base_key + "-end", def.end()));

    return validity;
}

std::chrono::system_clock::time_point
extract_with_default(const nlohmann::json &obj, const std::string &key,
                     const std::chrono::system_clock::time_point &tp)
{
    std::string date_str = extract_with_default(obj, key, "");
    if (date_str.length())
    {
        std::chrono::system_clock::time_point tp_out =
            std::chrono::system_clock::time_point::max();
        auto x = date::parse("%Y-%m-%dT%H:%M:%SZ", tp_out);
        std::istringstream iss(date_str);
        iss >> x;
        assert(iss.good());
        return tp_out;
    }
    return tp;
}
}
}
