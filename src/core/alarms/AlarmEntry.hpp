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

#include "core/alarms/AlarmFwd.hpp"
#include "core/alarms/IAlarmEntry.hpp"
#include "core/auth/AuthFwd.hpp"
#include "tools/ElapsedTimeCounter.hpp"
#include "tools/db/database.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <memory>
#include <odb/callback.hxx>
#include <odb/core.hxx>

namespace Leosac
{
namespace Alarms
{
/**
 * Implementation of IAlarmEntry, backed by ODB.
 *
 * This class, as well as all its child are backed by ODB, and are therefore
 * persisted in a SQL database.
 *
 * @see IAlarmEntry
  */
#pragma db object optimistic
class AlarmEntry : virtual public IAlarmEntry,
                   public std::enable_shared_from_this<AlarmEntry>
{
  protected:
    AlarmEntry();

  public:
    AlarmEntry(const AlarmEntry &) = delete;
    virtual ~AlarmEntry()          = default;

    static std::shared_ptr<AlarmEntry>  create(const DBPtr &database, const std::string& alarm_name, const AlarmType &type, const AlarmSeverity &severity, const std::string& reason);

    virtual AlarmEntryId id() const override;

    virtual bool finalized() const override;

    virtual void alarm(const std::string& alarm_name) override;

    virtual std::string alarm() const override;

    virtual void state(const AlarmState &state) override;

    virtual const AlarmState &state() const override;

    virtual void severity(const AlarmSeverity &severity) override;

    virtual const AlarmSeverity &severity() const override;

    virtual void type(const AlarmType &type) override;

    virtual const AlarmType &type() const override;

    virtual void author(Auth::UserPtr user) override;

    virtual Auth::UserId author_id() const override;

    virtual void reason(const std::string& r) override;

    virtual std::string reason() const override;

    virtual size_t version() const override;

    virtual void reload() override;

    virtual boost::posix_time::ptime timestamp() const override;

    virtual std::string generate_description() const override;

    /**
     * Set the database pointer.
     *
     * This is used by the DBService.
     */
    void database(DBPtr db);

  private:
#pragma db id auto
    AlarmEntryId id_;

#pragma db not_null
    boost::posix_time::ptime timestamp_;

    protected:
#pragma db not_null
    std::string alarm_;

    /**
     * The user at the source of the entry.
     * May be null.
     */
    Auth::UserLPtr author_;

    std::string reason_;

    AlarmState state_;

    AlarmSeverity severity_;

    AlarmType type_;

    /**
     * How long did it take for the Alarm object to be finalized.
     */
    size_t duration_;

/**
 * Pointer to the database.
 * Required to implement `finalize()`.
 *
 * Manually set by AlarmFactory.
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
};

}
}

#ifdef ODB_COMPILER
#include "core/auth/User.hpp"
#endif
