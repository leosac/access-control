/*
    Copyright (C) 2014-2015 Islog

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

#include <iomanip>
#include <core/auth/Auth.hpp>
#include "WiegandReaderImpl.hpp"
#include "tools/log.hpp"
#include "strategies/WiegandStrategy.hpp"

using namespace Leosac::Module::Wiegand;
using namespace Leosac::Module::Wiegand::Strategy;
using namespace Leosac::Hardware;
using namespace Leosac::Auth;

WiegandReaderImpl::WiegandReaderImpl(zmqpp::context &ctx,
        const std::string &reader_name,
        const std::string &data_high_pin,
        const std::string &data_low_pin,
        const std::string &green_led_name,
        const std::string &buzzer_name,
        std::unique_ptr<WiegandStrategy> strategy) :
        bus_sub_(ctx, zmqpp::socket_type::sub),
        sock_(ctx, zmqpp::socket_type::rep),
        bus_push_(ctx, zmqpp::socket_type::push),
        counter_(0),
        name_(reader_name),
        green_led_(nullptr),
        buzzer_(nullptr),
        strategy_(std::move(strategy))
{
    bus_sub_.connect("inproc://zmq-bus-pub");
    bus_push_.connect("inproc://zmq-bus-pull");

    sock_.bind("inproc://" + name_);

    topic_high_ = "S_INT:" + data_high_pin;
    topic_low_ = "S_INT:" + data_low_pin;

    bus_sub_.subscribe(topic_high_);
    bus_sub_.subscribe(topic_low_);

    std::fill(buffer_.begin(), buffer_.end(), 0);

    if (!green_led_name.empty())
        green_led_ = std::unique_ptr<FLED>(new FLED(ctx, green_led_name));

    if (!buzzer_name.empty())
        buzzer_ = std::unique_ptr<FBuzzer>(new FBuzzer(ctx, buzzer_name));
}

WiegandReaderImpl::~WiegandReaderImpl()
{
}

WiegandReaderImpl::WiegandReaderImpl(WiegandReaderImpl &&o) :
        bus_sub_(std::move(o.bus_sub_)),
        sock_(std::move(o.sock_)),
        bus_push_(std::move(o.bus_push_)),
        name_(std::move(o.name_)),
        strategy_(std::move(o.strategy_))
{
    topic_high_ = o.topic_high_;
    topic_low_ = o.topic_low_;

    buffer_ = o.buffer_;
    counter_ = o.counter_;

    green_led_ = std::move(o.green_led_);
    buzzer_ = std::move(o.buzzer_);

    // when we are moved, we must update our strategy's pointer back to the "new" us.
    strategy_->set_reader(this);
}

void WiegandReaderImpl::handle_bus_msg()
{
    std::string msg;
    bus_sub_.receive(msg);

    if (counter_ < 128)
    {
        if (msg == topic_high_)
        {
            buffer_[counter_ / 8] |= (1 << (7 - counter_ % 8));
        }
        else if (msg == topic_low_)
        {
            // set the bit to 0. it doesn't cost much
            // and is safer in case the buffer wasn't fully filled with 0.
            buffer_[counter_ / 8] &= ~(1 << (7 - counter_ % 8));
        }
        counter_++;
    }
    else
    {
        WARN("Received too many interrupt. Resetting current counter.");
        counter_ = 0;
    }
}

void WiegandReaderImpl::timeout()
{
    assert(strategy_);
    strategy_->timeout();

    if (strategy_->completed())
    {
        // if we gathered all the data we need, send
        // and authentication attempt by signaling the application.
        strategy_->signal(bus_push_);
        strategy_->reset();
    }
}

void WiegandReaderImpl::handle_request()
{
    zmqpp::message msg;
    std::string str;
    sock_.receive(msg);

    msg >> str;
    assert(str == "GREEN_LED" || str == "BEEP" || str == "BEEP_ON" || str == "BEEP_OFF");
    if (str == "GREEN_LED")
    {
        msg.pop_front();
        if (!green_led_)
        {
            sock_.send("KO");
            return;
        }
        // forward the request to the led.
        green_led_->backend().send(msg);
        green_led_->backend().receive(str);
        assert(str == "OK" || str == "KO");
        sock_.send(str == "OK" ? "OK" : "KO");
    }
    else if (str == "BEEP")
    {
        assert (msg.parts() == 2);
        int64_t duration;
        msg >> duration;
        if (!buzzer_)
        {
            sock_.send("KO");
            return;
        }
        bool ret = buzzer_->turnOn(std::chrono::milliseconds(duration));
        assert(ret);
        sock_.send("OK");
    }
    else if (str == "BEEP_ON")
    {
        if (!buzzer_)
        {
            sock_.send("KO");
            return;
        }
        bool ret = buzzer_->turnOn();
        assert(ret);
        sock_.send("OK");
    }
    else if (str == "BEEP_OFF")
    {
        if (!buzzer_)
        {
            sock_.send("KO");
            return;
        }
        bool ret = buzzer_->turnOff();
        assert(ret);
        sock_.send("OK");
    }
}

void WiegandReaderImpl::read_reset()
{
    counter_ = 0;
    std::fill(buffer_.begin(), buffer_.end(), 0);
}

const unsigned char *WiegandReaderImpl::buffer() const
{
    return &buffer_[0];
}

int WiegandReaderImpl::counter() const
{
    return counter_;
}

std::string const &WiegandReaderImpl::name() const
{
    return name_;
}
