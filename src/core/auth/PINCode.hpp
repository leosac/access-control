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
#include <sstream>
#include <string>

namespace Leosac
{
namespace Auth
{
class PINCode;

using PINCodePtr = std::shared_ptr<PINCode>;

/**
* A wiegand PIN code
*/
class PINCode : public BaseAuthSource
{
  public:
    MAKE_VISITABLE();

    /**
    * Create a PINCode object.
    *
    * @param pin the code in string format.
    */
    PINCode(const std::string &pin)
        : pin_code_(pin)
    {
    }

    const std::string &pin_code() const
    {
        return pin_code_;
    }

    virtual std::string to_string() const override
    {
        std::stringstream ss;

        ss << "Text representation of auth source:" << std::endl << "\t\t";
        ss << "Source Name: " << source_name_ << std::endl << "\t\t";
        ss << "Source Type: "
           << "PIN Code" << std::endl
           << "\t\t";
        ss << "Pin Code: " << pin_code_;
        return ss.str();
    }

  protected:
    std::string pin_code_;
};
}
}
