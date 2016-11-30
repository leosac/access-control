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

#include "SysFSGPIOPin.hpp"
#include "tools/unixfs.hpp"
#include <fcntl.h>
#include <tools/log.hpp>
#include <unistd.h>

using namespace Leosac::Module::SysFsGpio;
using Leosac::Tools::UnixFs;

SysFsGpioPin::SysFsGpioPin(zmqpp::context &ctx, const std::string &name, int gpio_no,
                           Direction direction, InterruptMode interrupt_mode,
                           bool initial_value, SysFsGpioModule &module)
    : gpio_no_(gpio_no)
    , sock_(ctx, zmqpp::socket_type::rep)
    , name_(name)
    , direction_(direction)
    , initial_value_(initial_value)
    , module_(module)
    , path_cfg_(module.general_config())
    , next_update_time_(std::chrono::system_clock::time_point::max())
{
    sock_.bind("inproc://" + name);

    set_direction(direction);
    set_interrupt(interrupt_mode);
    std::string full_path = path_cfg_.value_path(gpio_no);

    if (direction == Direction::Out)
    {
        if (initial_value_)
            turn_on();
        else
            turn_off();
    }

    file_fd_ = open(full_path.c_str(), O_RDONLY | O_NONBLOCK);
    assert(file_fd_ != -1);
}

SysFsGpioPin::~SysFsGpioPin()
{
    if (direction_ == Direction::Out)
    {
        if (initial_value_)
            turn_on();
        else
            turn_off();
    }

    if (file_fd_ != -1 && ::close(file_fd_) != 0)
    {
        ERROR("fail to close fd " << file_fd_);
    }
    try
    {
        UnixFs::writeSysFsValue(module_.general_config().unexport_path(), gpio_no_);
    }
    catch (FsException &e)
    {
        ERROR("Error while unexporting GPIO: " << e.what());
    }
}

void SysFsGpioPin::set_direction(Direction dir)
{
    std::string direction = dir == Direction::In ? "in" : "out";
    UnixFs::writeSysFsValue(path_cfg_.direction_path(gpio_no_), direction);
}

void SysFsGpioPin::set_interrupt(InterruptMode mode)
{
    std::string value;
    if (mode == SysFsGpioPin::InterruptMode::None)
        value = "none";
    else if (mode == SysFsGpioPin::InterruptMode::Both)
        value = "both";
    else if (mode == SysFsGpioPin::InterruptMode::Falling)
        value = "falling";
    else if (mode == SysFsGpioPin::InterruptMode::Rising)
        value = "rising";
    else
        assert(0);
    UnixFs::writeSysFsValue(path_cfg_.edge_path(gpio_no_), value);
}

void SysFsGpioPin::handle_message()
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

bool SysFsGpioPin::turn_on(zmqpp::message *msg /* = nullptr */)
{
    DEBUG("Remaining = " << msg->remaining());
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
    UnixFs::writeSysFsValue(path_cfg_.value_path(gpio_no_), 1);
    return true;
}

bool SysFsGpioPin::turn_off()
{
    UnixFs::writeSysFsValue(path_cfg_.value_path(gpio_no_), 0);
    return true;
}

bool SysFsGpioPin::toggle()
{
    int v = UnixFs::readSysFsValue<int>(path_cfg_.value_path(gpio_no_));
    UnixFs::writeSysFsValue(path_cfg_.value_path(gpio_no_), v == 1 ? 0 : 1);
    return true;
}

bool SysFsGpioPin::read_value()
{
    return UnixFs::readSysFsValue<bool>(path_cfg_.value_path(gpio_no_));
}

void SysFsGpioPin::handle_interrupt()
{
    std::array<char, 64> buffer;
    ssize_t ret;

    // flush interrupt by reading.
    // if we fail we cant recover, this means hardware failure.
    ret = ::read(file_fd_, &buffer[0], buffer.size());
    ASSERT_LOG(ret >= 0, "Read failed on GPIO pin.");
    ret = ::lseek(file_fd_, 0, SEEK_SET);
    ASSERT_LOG(ret >= 0, "Lseek failed on GPIO pin.");

    module_.publish_on_bus(zmqpp::message() << "S_INT:" + name_);
}

void SysFsGpioPin::register_sockets(zmqpp::reactor *reactor)
{
    reactor->add(sock_, std::bind(&SysFsGpioPin::handle_message, this));
    if (direction_ == Direction::In)
        reactor->add(file_fd_, std::bind(&SysFsGpioPin::handle_interrupt, this),
                     zmqpp::poller::poll_pri);
}

std::chrono::system_clock::time_point SysFsGpioPin::next_update() const
{
    return next_update_time_;
}

void SysFsGpioPin::update()
{
    DEBUG("Turning off SysFsGPIO pin.");
    turn_off();
    next_update_time_ = std::chrono::system_clock::time_point::max();
}
