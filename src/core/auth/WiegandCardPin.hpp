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

#include "core/auth/BaseAuthSource.hpp"
#include "core/auth/PINCode.hpp"
#include "core/auth/WiegandCard.hpp"
#include <string>
#include <tools/IVisitor.hpp>

namespace Leosac
{
namespace Auth
{
class WiegandCardPin;
using WiegandCardPinPtr = std::shared_ptr<WiegandCardPin>;

/**
* Credentials composed of a Wiegand card and a PIN code.
*/
class WiegandCardPin : public BaseAuthSource
{
  public:
    /**
    * Create a WiegandCardPin object.
    *
    * @param pin the code in string format.
    */
    WiegandCardPin(const std::string &card_id, int nb_bits,
                   const std::string &pin_code);

    virtual void accept(Tools::IVisitor *visitor) override
    {
        visitor->visit(this);
    }

    const WiegandCard &card() const;
    const PINCode &pin() const;

    virtual std::string to_string() const override;

  protected:
    WiegandCard card_;
    PINCode pin_;
};
}
}
