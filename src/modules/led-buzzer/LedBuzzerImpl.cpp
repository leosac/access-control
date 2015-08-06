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

#include "LedBuzzerImpl.hpp"
#include "tools/log.hpp"

using namespace Leosac::Module::LedBuzzer;

LedBuzzerImpl::LedBuzzerImpl(zmqpp::context &ctx,
                             std::string const &led_name,
                             std::string const &gpio_name,
                             int blink_duration,
                             int blink_speed) :
        ctx_(ctx),
        frontend_(ctx, zmqpp::socket_type::rep),
        backend_(ctx, zmqpp::socket_type::req),
        gpio_(ctx, gpio_name),
        default_blink_duration_(blink_duration),
        default_blink_speed_(blink_speed),
        stmachine_(std::ref(gpio_))
{
    frontend_.bind("inproc://" + led_name);
    backend_.connect("inproc://" + gpio_name);

    stmachine_.start();
}

zmqpp::socket &LedBuzzerImpl::frontend()
{
    return frontend_;
}

void LedBuzzerImpl::handle_message()
{
    zmqpp::message_t msg;
    zmqpp::message_t rep;
    std::string frame1;

    frontend_.receive(msg);
    msg >> frame1;
    bool ok = false;
    if (frame1 == "STATE")
    {
        return send_state();
    }
    if (frame1 == "ON" || frame1 == "OFF" || frame1 == "TOGGLE")
    {
        // simply forward message to GPIO
        rep = send_to_backend(msg);
        frontend_.send(rep);
        return;
    }
    else if (frame1 == "BLINK")
        ok = start_blink(&msg);
    else if (frame1 == "FAST_TO_SLOW")
    {
        ok = true;
        SM::EventPlayingPattern e;
        e.pattern = {
                {5000, 1000},
                {2100, 700},
                {1000, 100},
        };
        stmachine_.process_event(e);
    }
    else // invalid cmd
        assert(0);
    frontend_.send(ok ? "OK" : "KO");
}

void LedBuzzerImpl::update()
{
    DEBUG("UPDATING LED");
    stmachine_.process_event(SM::EventUpdate());
}

std::chrono::system_clock::time_point LedBuzzerImpl::next_update()
{
    return stmachine_.next_update();
}

zmqpp::message LedBuzzerImpl::send_to_backend(zmqpp::message &msg)
{
    zmqpp::message rep;
    backend_.send(msg);

    backend_.receive(rep);
    return rep;
}

bool LedBuzzerImpl::start_blink(zmqpp::message *msg)
{
    std::string tmp;
    SM::EventBlink event_blink;
    event_blink.duration = default_blink_duration_;
    event_blink.speed = default_blink_speed_;

    if (msg->parts() > 1)
    {
        *msg >> event_blink.duration;
    }

    if (msg->parts() > 2)
    {
        *msg >> event_blink.speed;
    }
    assert(event_blink.speed <= event_blink.duration);
    stmachine_.process_event(event_blink);

    return true;
}

void LedBuzzerImpl::send_state()
{
    zmqpp::message st;
    if (stmachine_.led_state_.st == Hardware::FLED::State::BLINKING)
    {
        st << "BLINKING";
        st << static_cast<int64_t>(stmachine_.led_state_.duration)
           << static_cast<int64_t>(stmachine_.led_state_.speed);
    }
    st << (gpio_.isOn() ? "ON" : "OFF");
    frontend_.send(st);
}
