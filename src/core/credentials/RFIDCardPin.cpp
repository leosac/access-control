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

#include "core/credentials/RFIDCardPin.hpp"
#include "tools/log.hpp"

namespace Leosac
{
namespace Cred
{

RFIDCardPin::RFIDCardPin()
    : RFIDCardPin(std::make_shared<RFIDCard>(), std::make_shared<PinCode>())
{
}

RFIDCardPin::RFIDCardPin(RFIDCardPtr card, PinCodePtr pin)
    : card_(card)
    , pin_(pin)
{
    ASSERT_LOG(card, "Cannot create an RFIDCardPin with null RFIDCard");
    ASSERT_LOG(pin, "Cannot create an RFIDCardPin with null PinCode");
}

const PinCode &RFIDCardPin::pin() const
{
    return *pin_;
}

const RFIDCard &RFIDCardPin::card() const
{
    return *card_;
}

RFIDCard &RFIDCardPin::card()
{
    return *card_;
}

PinCode &RFIDCardPin::pin()
{
    return *pin_;
}
}
}
