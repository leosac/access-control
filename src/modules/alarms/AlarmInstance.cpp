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

#include "modules/alarms/AlarmInstance.hpp"
#include "modules/alarms/AlarmEntry_odb.h"
#include "tools/db/OptionalTransaction.hpp"
#include "tools/log.hpp"
#include <core/auth/Auth.hpp>
#include <nlohmann/json.hpp>
#include <iomanip>

using namespace Leosac::Module::Alarms;
using namespace Leosac::Auth;

AlarmInstance::AlarmInstance(zmqpp::context &ctx,
                               DBPtr db,
                               std::shared_ptr<const Leosac::Hardware::Alarm> config,
                               const std::string& gpio_name)
    : sock_(ctx, zmqpp::socket_type::rep)
    , database_(db)
    , bus_push_(ctx, zmqpp::socket_type::push)
    , gpio_(ctx, gpio_name)
    , config_(config)
    , raise_counter_(0)
{
  bus_push_.connect("inproc://zmq-bus-pull");

  sock_.bind("inproc://" + config->name());
}

void AlarmInstance::register_sockets(zmqpp::reactor *reactor)
{
  reactor->add(sock_,
               std::bind(&AlarmInstance::handle_request, this));
}

std::string AlarmInstance::raise(Leosac::Hardware::AlarmType type, const std::string& reason)
{
  std::string alarm = "single";
  if (database_)
  {
    auto entry = AlarmEntry::create(database_, name(), type, config_->severity(), reason);
    alarm = std::to_string(entry->id());
  }

  INFO("Alarm {" << name() << "/" << alarm << "} created.");

  changeState(alarm, Leosac::Hardware::AlarmState::STATE_RAISED);
  return alarm;
}

void AlarmInstance::disarm(const std::string& alarm)
{
  changeState(alarm, Leosac::Hardware::AlarmState::STATE_DISARMED);
}

void AlarmInstance::changeState(const std::string& alarm, Leosac::Hardware::AlarmState state)
{
  INFO("Alarm {" << name() << "/" << alarm << "} changed state to " << static_cast<uint8_t>(state));

  if (database_)
  {
    auto entry = database_->load<AlarmEntry>(std::stoull(alarm));
    ASSERT_LOG(entry->state() == Leosac::Hardware::AlarmState::STATE_RAISED, "The alarm wasn't raised.");
    db::OptionalTransaction t(database_->begin());
    entry->state(state);
    t.commit();
  }

  handleGPIO(state);
}

void AlarmInstance::handleGPIO(Leosac::Hardware::AlarmState state)
{
  std::lock_guard<std::mutex> guard(raise_mutex_);
  if (state == Leosac::Hardware::AlarmState::STATE_RAISED)
  {
    raise_counter_++;
    if (raise_counter_ == 1 && !gpio_.name().empty())
    {
      gpio_.turnOn();
    }
  }
  else if (state == Leosac::Hardware::AlarmState::STATE_DISARMED || state == Leosac::Hardware::AlarmState::STATE_RESOLVED)
  {
    if (raise_counter_ > 0)
    {
      raise_counter_--;
      if (raise_counter_ == 0 && !gpio_.name().empty())
      {
        gpio_.turnOff();
      }
    }
  }
}

Leosac::Hardware::AlarmState AlarmInstance::getState(const std::string& alarm)
{
  Leosac::Hardware::AlarmState state = Leosac::Hardware::AlarmState::STATE_UNKNOWN;

  if (database_)
  {
    auto entry = database_->load<AlarmEntry>(std::stoull(alarm));
    state = entry->state();
  }
  return state;
}

void AlarmInstance::handle_request()
{
  zmqpp::message msg;
  std::string str;
  sock_.receive(msg);

  msg >> str;
  if (str == "RAISE" && msg.parts() == 3)
  {
    uint8_t type;
    std::string reason;
    msg >> type >> reason;
    try
    {
      auto alarm = raise(static_cast<Leosac::Hardware::AlarmType>(type), reason);
      zmqpp::message rep;
      rep << "OK" >> alarm;
      sock_.send(rep);
    }
    catch(const std::exception& e)
    {
      ERROR(e.what());
      sock_.send("KO");
    }
  }
  else if (str == "GET_STATE" && msg.parts() == 2)
  {
    try
    {
      std::string alarm;
      msg >> alarm;
      zmqpp::message rep;
      auto state = static_cast<uint8_t>(getState(alarm));
      rep << "OK" >> state;
      sock_.send(rep);
    }
    catch(const std::exception& e)
    {
      ERROR(e.what());
      sock_.send("KO");
    }
  }
  else if (str == "SET_STATE" && msg.parts() == 3)
  {
    try
    {
      std::string alarm;
      uint8_t state;
      msg >> alarm >> state;
      changeState(alarm, static_cast<Leosac::Hardware::AlarmState>(state));
      sock_.send("OK");
    }
    catch(const std::exception& e)
    {
      ERROR(e.what());
      sock_.send("KO");
    }
  }
  else
  {
    ERROR("Unrecognized request.");
    sock_.send("KO");
  }
}

const std::string& AlarmInstance::name() const
{
    return config_->name();
}

std::shared_ptr<const Leosac::Hardware::Alarm> AlarmInstance::config() const
{
  return config_;
}
