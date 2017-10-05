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

#include "PFDigitalPin.hpp"
#include "pifacedigital.h"
#include "tools/log.hpp"

PFDigitalPin::PFDigitalPin(zmqpp::context &ctx, const std::string &name, int gpio_no,
                           Direction direction, bool value, uint8_t hardware_address)
    : gpio_no_(gpio_no)
    , sock_(ctx, zmqpp::socket_type::rep)
    , bus_push_(new zmqpp::socket(ctx, zmqpp::socket_type::push))
    , name_(name)
    , direction_(direction)
    , default_value_(value)
    , hardware_address_(hardware_address)
    , want_update_(false)
{
    DEBUG("trying to bind to " << ("inproc://" + name));
    sock_.bind("inproc://" + name);
    bus_push_->connect("inproc://zmq-bus-pull");

    if (direction == Direction::Out)
        value ? turn_on() : turn_off();
}

PFDigitalPin::~PFDigitalPin()
{
    if (direction_ == Direction::Out)
        default_value_ ? turn_on() : turn_off();
    delete bus_push_;
}

PFDigitalPin::PFDigitalPin(PFDigitalPin &&o)
    : sock_(std::move(o.sock_))
    , direction_(o.direction_)
    , default_value_(o.default_value_)
{
    this->gpio_no_          = o.gpio_no_;
    this->name_             = o.name_;
    this->bus_push_         = o.bus_push_;
    this->want_update_      = o.want_update_;
    this->hardware_address_ = o.hardware_address_;

    o.bus_push_ = nullptr;
}

void PFDigitalPin::handle_message()
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
    else if (frame1 == "STATE")
        return send_state();
    else // invalid cmd
        ERROR("Invalid command received (" << frame1
                                           << "). Potential missconfiguration !");
    sock_.send(ok ? "OK" : "KO");
}

bool PFDigitalPin::turn_on(zmqpp::message *msg /* = nullptr */)
{
    if (direction_ != Direction::Out)
        return false;

    if (msg && msg->parts() > 1)
    {
        // optional parameter is present
        int64_t duration;
        *msg >> duration;
        next_update_time_ =
            std::chrono::system_clock::now() + std::chrono::milliseconds(duration);
        want_update_ = true;
    }
    pifacedigital_write_bit(1, gpio_no_, OUTPUT, hardware_address_);

    publish_state();
    return true;
}

bool PFDigitalPin::turn_off()
{
    if (direction_ != Direction::Out)
        return false;
    pifacedigital_write_bit(0, gpio_no_, OUTPUT, hardware_address_);

    publish_state();
    return true;
}

bool PFDigitalPin::toggle()
{
    if (direction_ != Direction::Out)
        return false;

    uint8_t v = pifacedigital_read_bit(gpio_no_, OUTPUT, hardware_address_);

    if (v)
        pifacedigital_write_bit(0, gpio_no_, OUTPUT, hardware_address_);
    else
        pifacedigital_write_bit(1, gpio_no_, OUTPUT, hardware_address_);

    publish_state();
    return true;
}

bool PFDigitalPin::read_value()
{
    // pin's direction matter here (not read from same register).
    return pifacedigital_read_bit(
        gpio_no_, direction_ == Direction::Out ? OUTPUT : INPUT, hardware_address_);
}

void PFDigitalPin::update()
{
    DEBUG("UPDATING");
    turn_off();
    want_update_ = false;
}

std::chrono::system_clock::time_point PFDigitalPin::next_update() const
{
    if (want_update_)
        return next_update_time_;
    return std::chrono::system_clock::time_point::max();
}

void PFDigitalPin::publish_state()
{
    if (bus_push_)
        bus_push_->send(zmqpp::message() << ("S_" + name_)
                                         << (read_value() ? "ON" : "OFF"));
}

void PFDigitalPin::send_state()
{
    sock_.send((read_value() ? "ON" : "OFF"));
}
