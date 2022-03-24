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

#include "core/credentials/Credential.hpp"
#include "core/credentials/IPinCode.hpp"

namespace Leosac
{
namespace Cred
{
/**
 * A PinCode credential.
 */
#pragma db object polymorphic optimistic
class PinCode : public virtual IPinCode, public Credential
{
  public:
    MAKE_VISITABLE_FALLBACK(IPinCode);
    PinCode() = default;

    const std::string &pin_code() const override;

    void pin_code(const std::string &string) override;

  protected:
    std::string pin_code_;

    friend class odb::access;
};

class PinCodeValidator
{
  public:
    static void validate(const IPinCode &card);
    static void validate_pin_code(const std::string &);
};
}
}
