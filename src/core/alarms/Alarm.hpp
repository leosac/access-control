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
#include "core/credentials/CredentialFwd.hpp"
#include "tools/db/database.hpp"
#include <zmqpp/socket.hpp>

namespace Leosac
{
/**
 * The Alarm namespace provides facilitate the raise, disarm and monitoring of an alarm.
 *
 * @note When Database is enabled, each alarm raise will result to a new AlarmEntry
 * for proper tracking of such important events.
 *
 * @note An alarm is considered as completed when its status goes to DISARM.
 *
 */
namespace Alarms
{
    /**
     * An alarm class to raise and disarm an alarm on Leosac.
     */
    class Alarm
    {
      public:
        Alarm(zmqpp::context &ctx, const DBPtr &database, const std::string& name, AlarmType type, AlarmSeverity severity);

        /**
         * Raise the alarm.
         */
        void raise(const std::string& reason, Auth::UserPtr user = nullptr);

        /**
         * Get the current alarm state.
         */
        AlarmState state() const;

        /**
         * Disarm the alarm.
         */
        void disarm();

      private:

        void changeState(AlarmState state);

        void sendmsg();

        std::string name_;

        AlarmType type_;

        AlarmSeverity severity_;

        AlarmState state_;

        Auth::UserPtr author_;

        std::string reason_;

        AlarmEntryPtr last_entry_;

        zmqpp::socket bus_push_;

        DBPtr database_;
    };
}
}
