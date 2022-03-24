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

#include "core/SecurityContext.hpp"
#include "core/auth/AuthFwd.hpp"
#include <nlohmann/json.hpp>

namespace Leosac
{
using json = nlohmann::json;

/**
 * A serializer that handle `Auth::Group` object.
 */
struct GroupJSONSerializer
{
    static json serialize(const Auth::Group &group, const SecurityContext &sc);

    static void unserialize(Auth::Group &out, const json &in,
                            const SecurityContext &sc);
};

struct GroupJSONStringSerializer
{
    static std::string serialize(const Auth::Group &in, const SecurityContext &sc);

    static void unserialize(Auth::Group &out, const std::string &in,
                            const SecurityContext &sc);
};
}
