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

#include "core/auth/IAccessPoint.hpp"

namespace Leosac
{
namespace Auth
{
#pragma db object optimistic
class AccessPoint : public virtual IAccessPoint
{
  public:
    AccessPoint();
    AccessPointId id() const override;

    const std::string &alias() const override;

    void alias(const std::string &new_alias) override;

    const std::string &description() const override;

    void description(const std::string &dsc) override;

  protected:
#pragma db id auto
    AccessPointId id_;
    std::string alias_;
    std::string description_;

#pragma db version
    const size_t version_;

  private:
    friend class odb::access;
};
}
}
