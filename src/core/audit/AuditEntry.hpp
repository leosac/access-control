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

#include "core/audit/AuditFwd.hpp"
#include "core/auth/AuthFwd.hpp"
#include "tools/db/database.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <odb/core.hxx>

namespace Leosac
{
namespace Audit
{
/**
 * An Audit entry.
 *
 * Leosac keeps an audit trail. This serves multiple purposes:
 *     + Additional security
 *     + Keep track of changes in order to generate differential configuration
 *     update to door devices.
 *
 * The audit log is sequential.
 */
#pragma db object polymorphic optimistic
class AuditEntry
{
  public:
    AuditEntry();

    virtual ~AuditEntry() = default;

#pragma db id auto
    unsigned long id_;

#pragma db not_null
    boost::posix_time::ptime timestamp_;

#pragma db not_null
    std::string msg_;

#pragma db value_not_null
    std::vector<AuditEntryPtr> children_;

    /**
     * The user at the source of the entry.
     * Maybe null.
     */
    Auth::UserLPtr author_;

#pragma db type("TEXT")
    EventMask event_mask_;

#pragma db version
    const ssize_t version_;

  private:
    friend class odb::access;
};
}
}

#ifdef ODB_COMPILER
#include "core/auth/User.hpp"
#endif
