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

#include "core/SecurityContext.hpp"
#include "core/auth/AuthFwd.hpp"
#include "tools/JSONSerializer.hpp"

namespace Leosac
{

/**
 * A serializer that handle `Auth::User` object.
 */
struct UserJSONSerializer : public JSONSerializer<Auth::User>
{
    static std::string to_string(const Auth::User &user, const SecurityContext &sc);
    static json to_object(const Auth::User &user, const SecurityContext &sc);
};
}
