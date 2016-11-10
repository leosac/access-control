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
#include "tools/log.hpp"

namespace Leosac
{

/**
 * A generic base template for serializer/deserialization.
 *
 * This class defines an "interface" for serialization. The implementation
 * of the interface is enforced by the use of CRTP: this base class will
 * invoke the `serialize()` and `unserialize()` method on the child class.
 *
 * @note `SerializedType` is the type of the object after serialization (string,
 * binary
 * blob/...)
 * @note `T` is the object under serialization.
 * @note `CRTP` is the type of the subclass deriving from the `Serializer` class.
 */
template <typename SerializedType, typename T, typename CRTP>
struct Serializer
{
    // todo: So apparently that was useless, remove and cleanup later.
    /*    static SerializedType serialize(const T &in, const SecurityContext &sc)
        {
            return CRTP::serialize(in, sc);
        }
        static void unserialize(T &out, const SerializedType &in,
                                const SecurityContext &sc)
        {
            CRTP::unserialize(out, in, sc);
        }*/
};
}
