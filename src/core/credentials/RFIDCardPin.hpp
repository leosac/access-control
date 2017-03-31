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

#pragma once

#include "core/credentials/PinCode.hpp"
#include "core/credentials/RFIDCard.hpp"
#include <string>

namespace Leosac
{
namespace Cred
{

/**
* Credentials composed of an RFIDCard and a PIN code.
 *
 * Todo: Consider making this class database aware so we can
 * configure those type of credential on the Web interface.
*/
class RFIDCardPin : public Credential
{
  public:
    MAKE_VISITABLE();

    /**
    * Create a RFIDCardPin object and assigned RFIDCard and
     * PinCode object to the RFIDCardPin credential
    */
    RFIDCardPin(RFIDCardPtr card, PinCodePtr pin);

    /**
     * Create an RFIDCardPin object with default
     * initialized RFIDCard and PinCode object.
     */
    RFIDCardPin();

    const RFIDCard &card() const;
    const PinCode &pin() const;

    RFIDCard &card();
    PinCode &pin();

  protected:
    RFIDCardPtr card_;
    PinCodePtr pin_;
};
}
}
