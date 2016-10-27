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

#include "core/audit/IAuditTracker.hpp"
#include "tools/db/database.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <memory>
#include <odb/core.hxx>

namespace Leosac
{
namespace Audit
{
/**
 * Implementation of IAuditTracker, backed by ODB.
 *
 * @see IAuditTracker
 */
#pragma db value
class AuditTracker // : virtual public IAuditTracker
{
  public:
    AuditTracker() = default;

    AuditTracker(const AuditEntry &) = delete;

    virtual ~AuditTracker() = default;

    virtual AuditEntryId last_id() const; // override;

  private:
    /**
     * The last audit entry we kept track of.
     */
    odb::lazy_shared_ptr<AuditEntry> last_;

    friend class odb::access;
};
}
}

#ifdef ODB_COMPILER
#include "core/audit/AuditEntry.hpp"
#endif
