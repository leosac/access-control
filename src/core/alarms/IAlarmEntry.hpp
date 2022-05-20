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
#include "core/auth/AuthFwd.hpp"
#include "tools/IVisitable.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <memory>

namespace Leosac
{
namespace Alarms
{
/**
 * Base interface to Alarm object.
 *
 * This interface is inherited by all "behavior specific" audit object, such as
 * IWSAPICall (when a websocket API call is made) or IUserEvent when something
 * happens to a user.
 *
 * There are 2 main reasons for using an interface here:
 *     + It reduces the required include for files that will use Alarm object.
 *       Indeed, only the interface file and the AlarmFactory header are required.
 *       No need to include various ODB headers.
 *     + Allows us the use of factory function to create Alarm object. This provides
 *       stronger garantee against bugs, as part of the object lifecycle is
 *       managed the factory function.
 */
class IAlarmEntry : public virtual Tools::IVisitable
{
  public:
    MAKE_VISITABLE();

    /**
     * Retrieve the identifier of the entry.
     *
     * An `id` of 0 means that the entry is not persisted yet.
     */
    virtual AlarmEntryId id() const = 0;

    /**
     * Is this entry finalized.
     */
    virtual bool finalized() const = 0;

    /**
     * Reload the object from the database.
     *
     * Pre-Conditions:
     *     + Must be in a database transaction.
     *
     * @note Be careful as internal change made to the object will be
     *       lost when `reload()`ing the object.
     */
    virtual void reload() = 0;

    /**
     * Set the alarm name to which this entry is related.
     * @param alarm_name
     */
    virtual void alarm(const std::string& alarm_name) = 0;

    /**
     * Retrieve the alarm name to which this entry is related.
     */
    virtual std::string alarm() const = 0;

    /**
     * Set the alarm state for the entry.
     * @param state
     */
    virtual void state(const AlarmState &state) = 0;

    /**
     * Retrieve the current alarm state.
     */
    virtual const AlarmState &state() const = 0;

    /**
     * Set the alarm severity for the entry.
     * @param severity
     */
    virtual void severity(const AlarmSeverity &severity) = 0;

    /**
     * Retrieve the current alarm severity.
     */
    virtual const AlarmSeverity &severity() const = 0;

    /**
     * Set the alarm type for the entry.
     * @param type
     */
    virtual void type(const AlarmType &type) = 0;

    /**
     * Retrieve the current alarm type.
     */
    virtual const AlarmType &type() const = 0;

    /**
     * Set the author of the entry.
     *
     * The author is the user that is responsible (aka at the source)
     * of the alarm.
     *
     * Pre-Conditions:
     *     + The `user` pointer, if not null, must point to an already persisted
     *     user object.
     */
    virtual void author(Auth::UserPtr user) = 0;

    /**
     * Retrieve the user id of the author of this entry.
     *
     * It may returns 0, which is not a valid user_id.
     */
    virtual Auth::UserId author_id() const = 0;

    /**
     * Set the reason related to the entry.
     */
    virtual void reason(const std::string& r) = 0;

    /**
     * Retrieve the reason related to this entry.
     *
     * It may returns empty.
     */
    virtual std::string reason() const = 0;

    /**
     * Retrieve unix timestamp on when the alarm has been raised.
     */
    virtual boost::posix_time::ptime timestamp() const = 0;

    /**
     * Generate a description for this event.
     */
    virtual std::string generate_description() const = 0;

    /**
     * Returns the ODB version of the object.
     *
     * This call is for debugging purpose.
     */
    virtual size_t version() const = 0;
};
}
}
