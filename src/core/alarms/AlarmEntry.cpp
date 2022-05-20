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

#include "AlarmEntry.hpp"
#include "core/alarms/AlarmEntry_odb.h"
#include "core/auth/User.hpp"
#include "core/auth/User_odb.h"
#include "tools/db/OptionalTransaction.hpp"
#include "tools/log.hpp"
#include <odb/query.hxx>

using namespace Leosac;
using namespace Leosac::Alarms;

AlarmEntry::AlarmEntry()
    : duration_(0)
    , state_(AlarmState::STATE_DEFAULT)
    , version_(0)
{
    timestamp_ = boost::posix_time::second_clock::local_time();
}

std::shared_ptr<AlarmEntry> AlarmEntry::create(const DBPtr &database,
                                             const std::string& alarm_name,
                                             const AlarmType &type,
                                             const AlarmSeverity &severity,
                                             const std::string& reason)
{
    ASSERT_LOG(database, "Database must be non null.");

    db::OptionalTransaction t(database->begin());
    auto alarm = std::shared_ptr<Alarms::AlarmEntry>(new Alarms::AlarmEntry());
    alarm->database(database);
    alarm->alarm(alarm_name);
    alarm->type(type);
    alarm->severity(severity);
    alarm->reason(reason);

    database->persist(alarm);
    t.commit();
    return alarm;
}

AlarmEntryId AlarmEntry::id() const
{
    return id_;
}

bool AlarmEntry::finalized() const
{
    return (state_ == AlarmState::STATE_DEFAULT || state_ == AlarmState::STATE_RAISED);
}

std::string AlarmEntry::alarm() const
{
    return alarm_;
}

void AlarmEntry::alarm(const std::string& alarm_name)
{
    alarm_ = alarm_name;
}

void AlarmEntry::state(const AlarmState &state)
{
    ASSERT_LOG(odb::transaction::has_current(),
              "Not currently in a database transaction.");

    ASSERT_LOG(!finalized(), "Alarm entry is already finalized.");
    state_ = state;

    ASSERT_LOG(database_, "Null database pointer for AlarmEntry.");
    duration_ += etc_.elapsed();
    database_->update(*this);
}

const AlarmState &AlarmEntry::state() const
{
    return state_;
}

void AlarmEntry::severity(const AlarmSeverity &severity)
{
    ASSERT_LOG(!finalized(), "Alarm entry is already finalized.");
    severity_ = severity;
}

const AlarmSeverity &AlarmEntry::severity() const
{
    return severity_;
}

void AlarmEntry::type(const AlarmType &type)
{
    ASSERT_LOG(!finalized(), "Alarm entry is already finalized.");
    type_ = type;
}

const AlarmType &AlarmEntry::type() const
{
    return type_;
}

void AlarmEntry::author(Auth::UserPtr user)
{
    if (user)
        ASSERT_LOG(user->id(), "Author is not already persisted.");
    author_ = user;
}

size_t AlarmEntry::version() const
{
    return version_;
}

void AlarmEntry::reload()
{
    ASSERT_LOG(odb::transaction::has_current(), "Not currently in transaction.");
    database_->reload(this);
}

void AlarmEntry::database(DBPtr db)
{
    database_ = db;
}

Auth::UserId AlarmEntry::author_id() const
{
    if (author_)
        return author_.object_id();
    return 0;
}

std::string AlarmEntry::reason() const
{
    return reason_;
}

void AlarmEntry::reason(const std::string& r)
{
    reason_ = r;
}

boost::posix_time::ptime AlarmEntry::timestamp() const
{
    return timestamp_;
}

std::string AlarmEntry::generate_description() const
{
    std::stringstream ss;

    ss << "Alarm" << alarm_ << " of type ";
    if (type_ == AlarmType::ALARM_FORCED)
        ss << "`forced`";
    else if (type_ == AlarmType::ALARM_UNAUTHORIZED_ACCESS)
        ss << "`unauthorized access`";
    else if (type_ == AlarmType::ALARM_OFFLINE)
        ss << "`marked as offline`";
    else if (type_ == AlarmType::ALARM_BATTERY_LOW)
        ss << "`low battery`";
    else if (type_ == AlarmType::ALARM_BATTERY_ACTIVATED)
        ss << "`switched to battery`";
    else if (type_ == AlarmType::ALARM_SUSPICIOUS_BEHAVIOR)
        ss << "`suspicious behavior`";

    ss << " (Reason:" << reason_ << ").";

    return ss.str();
}
