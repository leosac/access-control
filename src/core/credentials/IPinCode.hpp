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

#include "core/credentials/ICredential.hpp"
#include <memory>

namespace Leosac
{
namespace Cred
{
/**
 * Interface for PinCode credentials.
 */
class IPinCode : public virtual ICredential
{
  public:
    MAKE_VISITABLE();

    virtual const std::string &pin_code() const = 0;
    virtual void pin_code(const std::string &)  = 0;
};
}
}
