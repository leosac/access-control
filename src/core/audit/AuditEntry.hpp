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
#include "core/audit/IAuditEntry.hpp"
#include "core/auth/AuthFwd.hpp"
#include "tools/db/database.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <memory>
#include <odb/callback.hxx>
#include <odb/core.hxx>
#include <tools/ElapsedTimeCounter.hpp>

namespace Leosac
{
namespace Audit
{
/**
 * Implementation of IAuditEntry, backed by ODB.
 *
 * This class, as well as all its child are backed by ODB, and are therefore
 * persisted in a SQL database.
 *
 * @see IAuditEntry
  */
#pragma db object polymorphic optimistic callback(odb_callback)
class AuditEntry : virtual public IAuditEntry,
                   public std::enable_shared_from_this<AuditEntry>
{
  protected:
    AuditEntry();

  public:
    virtual ~AuditEntry() = default;

    virtual AuditEntryId id() const override;

    virtual void finalize() override;

    virtual bool finalized() const override;

    virtual void event_mask(const EventMask &mask) override;

    virtual const EventMask &event_mask() const override;

    virtual void author(Auth::UserPtr user) override;

    virtual void set_parent(IAuditEntryPtr parent) override;

    virtual IAuditEntryPtr parent() const override;

    virtual void remove_parent() override;

    virtual ssize_t children_count() const override;

    virtual ssize_t version() const override;

    virtual void reload() override;

  private:
#pragma db id auto
    AuditEntryId id_;

#pragma db not_null
    boost::posix_time::ptime timestamp_;

#pragma db value_not_null
    std::vector<AuditEntryPtr> children_;

  protected:
#pragma db inverse(children_)
    AuditEntryWPtr parent_;

#pragma db not_null
    std::string msg_;

    /**
     * The user at the source of the entry.
     * May be null.
     */
    Auth::UserLPtr author_;

#pragma db type("TEXT")
    EventMask event_mask_;

    /**
     * How long did it take for the Audit object to be finalized.
     */
    size_t duration_;

    /**
     * Audit entry are sometime persisted multiple time before
     * reaching their final state.
     *
     * When set to true, this flag indicates that the audit entry has reach its
     * final status. This is useful to detect incomplete audit entry.
     */
    bool finalized_;

/**
 * Pointer to the database.
 * Required to implement `finalize()`.
 *
 * Manualy set by Audit::Factory.
 */
#pragma db transient
    DBPtr database_;

/**
 * Keep track of how long the object has been alive.
 * This helps populates the `duration_` field.
 */
#pragma db transient
    Tools::ElapsedTimeCounter etc_;

  private:
#pragma db version
    const size_t version_;

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
