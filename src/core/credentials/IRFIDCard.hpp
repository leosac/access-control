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

#include "core/credentials/ICredential.hpp"
#include <memory>

namespace Leosac
{
namespace Cred
{
/**
 * Interface for RFIDCard credential.
 */
class IRFIDCard : public virtual ICredential
{
  public:
    MAKE_VISITABLE_FALLBACK(ICredential);

    virtual const std::string &card_id() const = 0;
    virtual int nb_bits() const                = 0;

    virtual void nb_bits(int)                 = 0;
    virtual void card_id(const std::string &) = 0;

    /**
     * Returns the integer representation of the
     * card ID.
     *
     * The format (Wiegand 26, 32, ....) is used to build the
     * card number. If no format is recognized, fallback to `to_raw_int()`
     */
    virtual uint64_t to_int() const = 0;

    /**
     * Convert the bits of the card to an integer.
     *
     * The format (Wiegand26, 32, ...) is ignored: all bits are used
     * to build the number.
     */
    virtual uint64_t to_raw_int() const = 0;
};
}
}
