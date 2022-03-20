/*
    Copyright (C) 2014-2016 Leosac

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

#include "AuthTarget.hpp"
#include "tools/log.hpp"

using namespace Leosac::Auth;

const std::string &AuthTarget::name() const
{
    return name_;
}

void AuthTarget::name(std::string const &param)
{
    INFO("Changing target name (from " << name_ << " to " << param);
    name_ = param;
}

AuthTarget::AuthTarget(const std::string target_name)
    : name_(target_name),
    contact_duration_(15000)
{
}

void AuthTarget::add_always_open_sched(Leosac::Tools::IScheduleCPtr const &sched)
{
    always_open_.push_back(sched);
}

void AuthTarget::add_always_close_sched(Leosac::Tools::IScheduleCPtr const &sched)
{
    always_close_.push_back(sched);
}

Leosac::Hardware::FGPIO *AuthTarget::gpio()
{
    return gpio_.get();
}

void AuthTarget::gpio(std::unique_ptr<Leosac::Hardware::FGPIO> new_gpio)
{
    gpio_ = std::move(new_gpio);
}

void AuthTarget::exitreq_gpio(std::unique_ptr<Leosac::Hardware::FGPIO> new_gpio)
{
    exitreq_gpio_ = std::move(new_gpio);
}

void AuthTarget::exitreq_duration(std::chrono::milliseconds duration)
{
    exitreq_duration_ = duration;
}

void AuthTarget::contact_gpio(std::unique_ptr<Leosac::Hardware::FGPIO> new_gpio)
{
    contact_gpio_ = std::move(new_gpio);
}

void AuthTarget::contact_duration(std::chrono::milliseconds duration)
{
    contact_duration_ = duration;
}

bool AuthTarget::is_always_open(
    const std::chrono::system_clock::time_point &tp) const
{
    for (const auto &sched : always_open_)
    {
        if (sched->is_in_schedule(tp))
            return true;
    }
    return false;
}

bool AuthTarget::is_always_closed(
    const std::chrono::system_clock::time_point &tp) const
{
    for (const auto &sched : always_close_)
    {
        if (sched->is_in_schedule(tp))
            return true;
    }
    return false;
}

void AuthTarget::resetToExpectedState(const std::chrono::system_clock::time_point &tp)
{
  if (is_always_open(tp) && is_always_closed(tp))
  {
      WARN("Oops, door "
           << name()
           << " is both always open and always close at the same time.");
  }
  else if (is_always_open(tp) && !gpio()->isOn())
  {
      gpio()->turnOn();
  }
  else if (is_always_closed(tp) && !gpio()->isOff())
  {
      gpio()->turnOff();
  }
}
