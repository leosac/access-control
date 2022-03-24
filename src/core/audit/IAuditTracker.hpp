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

#include "core/audit/AuditFwd.hpp"

namespace Leosac
{
namespace Audit
{
/**
 * The AuditTracker class provides the ability to "track" the audit log
 * and acknowledge its entries.
 *
 * It can be used to keep track of "how up-to-date" a component is with regards
 * to the audit log.
 */
class IAuditTracker
{
  public:
    /**
     * Returns the id the last processed audit entry.
     */
    virtual AuditEntryId last_id() const = 0;
};
}
}
