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

#include "core/credentials/PinCode.hpp"
#include "exception/ModelException.hpp"
#include "tools/log.hpp"
#include <boost/algorithm/string.hpp>

using namespace Leosac;
using namespace Leosac::Cred;

const std::string &PinCode::pin_code() const
{
    return pin_code_;
}

void PinCode::pin_code(const std::string &pin)
{
    PinCodeValidator::validate_pin_code(pin);
    pin_code_ = pin;
}

void PinCodeValidator::validate(const IPinCode &pin)
{
    validate_pin_code(pin.pin_code());
}

void PinCodeValidator::validate_pin_code(const std::string &pin)
{
    for (const auto &c : pin)
    {
        if (!(c >= '0' && c <= '9'))
        {
            throw ModelException(
                "data/attributes/code",
                BUILD_STR("Pin code contains invalid character: " << c));
        }
    }
}
