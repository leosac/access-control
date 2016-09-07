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

#include "AuditEntry.hpp"

namespace Leosac
{
namespace Audit
{
/**
 * An audit that keeps track of an UserEvent.
 *
 * UserEvent indicates that a user was modified (wether directly
 * or indirectly).
 */
#pragma db object polymorphic callback(odb_callback)
class UserEvent : public AuditEntry
{
  public:
    UserEvent() = default;

    virtual ~UserEvent() = default;

#pragma db not_null
    Auth::UserLWPtr target_;


  private:
    friend class odb::access;
};
}
}
