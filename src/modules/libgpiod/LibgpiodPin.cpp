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

#include "LibgpiodPin.hpp"
#include <fcntl.h>
#include <tools/log.hpp>
#include <unistd.h>

using namespace Leosac::Module::Libgpiod;

LibgpiodPin::LibgpiodPin(zmqpp::context &ctx, const std::string &name, const std::string &gpio_device,
                           int gpio_offset, Direction direction, InterruptMode interrupt_mode,
                           bool initial_value, LibgpiodModule &module)
    : sock_(ctx, zmqpp::socket_type::rep)
    , name_(name)
    , gpio_device_(gpio_device)
    , gpio_offset_(gpio_offset)
    , direction_(direction)
    , initial_value_(initial_value)
    , module_(module)
    , next_update_time_(std::chrono::system_clock::time_point::max())
    , gpiod_chip_(nullptr)
    , gpiod_line_(nullptr)
    , gpiod_fd_(-1)
{
    sock_.bind("inproc://" + name);

    gpiod_chip_ = gpiod_chip_open_lookup(gpio_device_.c_str());
    assert(gpiod_chip_ != nullptr);

    gpiod_line_ = gpiod_chip_get_line(gpiod_chip_, gpio_offset_);
    assert(gpiod_line_ != nullptr);

    set_direction(direction);
    set_interrupt(interrupt_mode);
}

LibgpiodPin::~LibgpiodPin()
{
  release();
}

void LibgpiodPin::release()
{
  if (gpiod_line_ != nullptr)
  {
    gpiod_line_release(gpiod_line_);
  }
  if (gpiod_chip_ != nullptr)
  {
    gpiod_chip_close(gpiod_chip_);
  }
}

void LibgpiodPin::set_direction(Direction dir)
{
    if (dir == Direction::In)
    {
      gpiod_line_request_input(gpiod_line_, module_.general_config()->consumer().c_str());
    }
    else
    {
      gpiod_line_request_output(gpiod_line_, module_.general_config()->consumer().c_str(), initial_value_);
    }
}

void LibgpiodPin::set_interrupt(InterruptMode mode)
{
  int ret;
  if (mode == LibgpiodPin::InterruptMode::None)
    ret = 0;
  else if (mode == LibgpiodPin::InterruptMode::Both)
    ret = gpiod_line_request_both_edges_events(gpiod_line_, module_.general_config()->consumer().c_str());
  else if (mode == LibgpiodPin::InterruptMode::Falling)
    ret = gpiod_line_request_falling_edge_events(gpiod_line_, module_.general_config()->consumer().c_str());
  else if (mode == LibgpiodPin::InterruptMode::Rising)
    ret = gpiod_line_request_rising_edge_events(gpiod_line_, module_.general_config()->consumer().c_str());
  else
    assert(0);

  ASSERT_LOG(ret >= 0, "Toggle failed on GPIO pin.");
}

void LibgpiodPin::handle_message()
{
    zmqpp::message_t msg;
    std::string frame1;
    sock_.receive(msg);

    msg >> frame1;
    bool ok = false;
    if (frame1 == "ON")
        ok = turn_on(&msg);
    else if (frame1 == "OFF")
        ok = turn_off();
    else if (frame1 == "TOGGLE")
        ok = toggle();
    sock_.send(ok ? "OK" : "KO");

    // publish new state.
    module_.publish_on_bus(zmqpp::message() << ("S_" + name_)
                                            << (read_value() ? "ON" : "OFF"));
}

bool LibgpiodPin::turn_on(zmqpp::message *msg /* = nullptr */)
{
    if (msg && msg->remaining() == 1)
    {
        // ASSERT_LOG(msg->parts() == 2 && msg->remaining() == 1, "Invalid internal
        // message.");
        // optional parameter is present
        int64_t duration;
        *msg >> duration;
        next_update_time_ =
            std::chrono::system_clock::now() + std::chrono::milliseconds(duration);
    }
    else if (msg)
    {
        WARN("Called with unexpected number of arguments: " << msg->remaining());
    }

    int ret = gpiod_line_set_value(gpiod_line_, 1);
    return (ret == 1);
}

bool LibgpiodPin::turn_off()
{
    int ret = gpiod_line_set_value(gpiod_line_, 0);
    return (ret == 1);
}

bool LibgpiodPin::toggle()
{
  int ret = gpiod_line_set_value(gpiod_line_, read_value() ? 1 : 0);
  ASSERT_LOG(ret >= 0, "Toggle failed on GPIO pin.");
  return (ret == 1);
}

bool LibgpiodPin::read_value()
{
  int ret = gpiod_line_get_value(gpiod_line_);
  ASSERT_LOG(ret >= 0, "Read failed on GPIO pin.");
  return (ret == 1);
}

void LibgpiodPin::handle_interrupt()
{
    gpiod_line_event gpiod_event;
    int ret = gpiod_line_event_read_fd(gpiod_fd_, &gpiod_event);
    ASSERT_LOG(ret >= 0, "Read failed on GPIO pin.");

    module_.publish_on_bus(zmqpp::message() << "S_INT:" + name_);
}

void LibgpiodPin::register_sockets(zmqpp::reactor *reactor)
{
    reactor->add(sock_, std::bind(&LibgpiodPin::handle_message, this));
    if (direction_ == Direction::In)
    {
        gpiod_fd_ = gpiod_line_event_get_fd(gpiod_line_);
        ASSERT_LOG(gpiod_fd_ >= 0, "Bad GPIO line or the line is not setup for event monitoring.");
        reactor->add(gpiod_fd_, std::bind(&LibgpiodPin::handle_interrupt, this),
                     zmqpp::poller::poll_pri);
    }
}

std::chrono::system_clock::time_point LibgpiodPin::next_update() const
{
    return next_update_time_;
}

void LibgpiodPin::update()
{
    DEBUG("Turning off Libgpiod pin.");
    turn_off();
    next_update_time_ = std::chrono::system_clock::time_point::max();
}
