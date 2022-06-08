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

#define ODB_NO_BASE_VERSION
#include "modules/alarms/AlarmsFwd.hpp"
#include "hardware/Alarm.hpp"
#include "hardware/facades/FAlarm.hpp"
#include "core/auth/AuthFwd.hpp"
#include "tools/ElapsedTimeCounter.hpp"
#include "tools/db/database.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <memory>
#include <odb/callback.hxx>
#include <odb/core.hxx>

#pragma db model version(1, 1, open)

namespace Leosac
{
namespace Module
{
namespace Alarms
{
/**
 * This class, as well as all its child are backed by ODB, and are therefore
 * persisted in a SQL database.
  */
#pragma db object optimistic table("HARDWARE_ALARM_ALARMENTRY")
class AlarmEntry
{
  protected:
    AlarmEntry();

  public:
    AlarmEntry(const AlarmEntry &) = delete;
    virtual ~AlarmEntry()          = default;

    static std::shared_ptr<AlarmEntry> create(const DBPtr &database, const std::string& alarm_name, const Hardware::AlarmType &type, const Hardware::Alarm::AlarmSeverity &severity, const std::string& reason);

    virtual AlarmEntryId id() const;

    virtual bool finalized() const;

    virtual void alarm(const std::string& alarm_name);

    virtual std::string alarm() const;

    virtual void state(const Hardware::AlarmState &state);

    virtual const Hardware::AlarmState &state() const;

    virtual void severity(const Hardware::Alarm::AlarmSeverity &severity);

    virtual const Hardware::Alarm::AlarmSeverity &severity() const;

    virtual void type(const Hardware::AlarmType &type);

    virtual const Hardware::AlarmType &type() const;

    virtual void reason(const std::string& r);

    virtual std::string reason() const;

    virtual size_t version() const;

    virtual boost::posix_time::ptime timestamp() const;

    virtual std::string generate_description() const;

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

    std::string reason_;

    Hardware::AlarmState state_;

    Hardware::Alarm::AlarmSeverity severity_;

    Hardware::AlarmType type_;

    /**
     * How long did it take for the Alarm object to be finalized.
     */
    size_t duration_;

/**
 * Pointer to the database.
 * Required to implement `finalize()`.
 *
 * Manually set.
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
}
