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
#include <memory>
#include <odb/callback.hxx>
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
#pragma db object polymorphic optimistic callback(odb_callback)
class AuditEntry : public std::enable_shared_from_this<AuditEntry>
{
  public:
    AuditEntry();

    virtual ~AuditEntry() = default;

    AuditEntryId id() const;

    /**
     * Set `parent` as the parent audit entry for
     * this entry.
     *
     * The `set_parent()` will copy the parent's author to this->author_
     * if there currently is no author assigned to the entry.
     *
     * Pre-Conditions:
     *     + Shall have no parent.
     *     + The `parent` must be a non-null, already persisted
     *       object.
     *
     * Post-Conditions:
     *     + Will have a parent.
     *     + This object will be somewhere in `parent->children_` array.
     *
     * @param parent
     */
    void set_parent(AuditEntryPtr parent);

#pragma db not_null
    std::string msg_;

    /**
     * The user at the source of the entry.
     * Maybe null.
     */
    Auth::UserLPtr author_;

#pragma db type("TEXT")
    EventMask event_mask_;

  private:
#pragma db not_null
    boost::posix_time::ptime timestamp_;

#pragma db value_not_null
    std::vector<AuditEntryPtr> children_;

#pragma db inverse(children_)
    AuditEntryWPtr parent_;

#pragma db id auto
    AuditEntryId id_;

#pragma db version
    const ssize_t version_;

    friend class odb::access;

    /**
     * Implementation of an ODB callback.
     *
     * The callback will make sure to update the parent (if any).
     * This is to ensure that parent-child relationship are persisted when saving
     * the child.
     *
     * Note that the reverse is not needed, because the child must always be fully
     * saved
     * before the parent gets its final update.
     */
    void odb_callback(odb::callback_event e, odb::database &) const;
};
}
}

#ifdef ODB_COMPILER
#include "core/auth/User.hpp"
#endif
