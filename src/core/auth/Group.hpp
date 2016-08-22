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

#include "core/auth/AuthFwd.hpp"
#include "tools/db/database.hpp"
#include <string>
#include <vector>

namespace Leosac
{
namespace Auth
{

/**
* A authentication group regroup users that share permissions.
*/
#pragma db object pointer(std::shared_ptr)
class Group
{
  public:
    explicit Group(const std::string &group_name);

    const std::string &name() const;

    const std::vector<UserPtr> &members() const;

    void member_add(UserPtr m);

    IAccessProfilePtr profile();

    void profile(IAccessProfilePtr p);

    GroupId id() const;

  private:
    friend class odb::access;
#pragma db id auto
    GroupId id_;
#pragma db transient
    std::vector<UserPtr> members_;
    std::string name_;

#pragma db transient
    IAccessProfilePtr profile_;
};
}
}