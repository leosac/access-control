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

#include "hardware/Alarm.hpp"
#include "hardware/facades/FAlarm.hpp"
#include "hardware/facades/FGPIO.hpp"
#include "tools/log.hpp"
#include "tools/db/database.hpp"
#include <chrono>
#include <zmqpp/zmqpp.hpp>
#include <mutex>

namespace Leosac
{
namespace Module
{
namespace Alarms
{
/**
* Implementation class, for use by the Alarm module only.
*/
class AlarmInstance
{
  public:
    /**
    * @param ctx ZMQ context
    * @param config configuration of the server.
    */
    AlarmInstance(zmqpp::context &ctx, DBPtr db, std::shared_ptr<const Hardware::Alarm> config, const std::string& gpio_name);

    AlarmInstance(AlarmInstance&) = delete;

    void register_sockets(zmqpp::reactor *reactor);

    std::string raise(Hardware::AlarmType type, const std::string& reason);

    void disarm(const std::string& alarm);

    void changeState(const std::string& alarm, Hardware::AlarmState state);

    Hardware::AlarmState getState(const std::string& alarm);

    /**
    * Someone sent a request.
    */
    void handle_request();

    std::shared_ptr<const Hardware::Alarm> config() const;

    const std::string& name() const;

  private:

    void handleGPIO(Leosac::Hardware::AlarmState state);

    /**
    * REP socket to receive command on.
    */
    zmqpp::socket sock_;

    DBPtr database_;

    /**
    * Socket to write to the message bus.
    */
    zmqpp::socket bus_push_;

    /**
    * Facade to the GPIO we use with this Alarm (optional).
    */
    Hardware::FGPIO gpio_;

    std::shared_ptr<const Hardware::Alarm> config_;

    std::atomic<unsigned int> raise_counter_;

    std::mutex raise_mutex_;
};
}
}
}
