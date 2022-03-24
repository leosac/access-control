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

#include "FLED.hpp"
#include <tools/log.hpp>
#include <zmqpp/message.hpp>

using namespace Leosac::Hardware;

FLED::FLED(zmqpp::context &ctx, const std::string &led_name)
    : backend_(ctx, zmqpp::socket_type::req)
{
    backend_.connect("inproc://" + led_name);
    poller_.add(backend_);
}

bool FLED::turnOn(std::chrono::milliseconds duration)
{
    std::string rep;
    zmqpp::message msg;

    msg << "ON" << duration.count();

    backend_.send(msg);
    poller_.poll(5000);
    ASSERT_LOG(poller_.has_input(backend_), "Operation was blocked.");
    backend_.receive(rep);
    if (rep == "OK")
        return true;
    return false;
}

bool FLED::turnOn(int duration)
{
    return turnOn(std::chrono::milliseconds(duration));
}

bool FLED::turnOn()
{
    std::string rep;

    backend_.send("ON");
    poller_.poll(5000);
    ASSERT_LOG(poller_.has_input(backend_), "Operation was blocked.");
    backend_.receive(rep);
    if (rep == "OK")
        return true;
    return false;
}

bool FLED::turnOff()
{
    std::string rep;

    backend_.send("OFF");
    poller_.poll(5000);
    ASSERT_LOG(poller_.has_input(backend_), "Operation was blocked.");
    backend_.receive(rep);
    if (rep == "OK")
        return true;
    return false;
}

bool FLED::toggle()
{
    std::string rep;

    backend_.send("TOGGLE");
    poller_.poll(5000);
    ASSERT_LOG(poller_.has_input(backend_), "Operation was blocked.");
    backend_.receive(rep);
    if (rep == "OK")
        return true;
    return false;
}

bool FLED::blink()
{
    std::string rep;

    backend_.send("BLINK");
    poller_.poll(5000);
    ASSERT_LOG(poller_.has_input(backend_), "Operation was blocked.");
    backend_.receive(rep);
    if (rep == "OK")
        return true;
    return false;
}

bool FLED::blink(std::chrono::milliseconds duration, std::chrono::milliseconds speed)
{
    std::string rep;
    zmqpp::message msg;

    msg << "BLINK" << duration.count() << speed.count();

    backend_.send(msg);
    poller_.poll(5000);
    ASSERT_LOG(poller_.has_input(backend_), "Operation was blocked.");
    backend_.receive(rep);
    if (rep == "OK")
        return true;
    return false;
}

bool FLED::blink(int duration, int speed)
{
    return blink(std::chrono::milliseconds(duration),
                 std::chrono::milliseconds(speed));
}

bool FLED::isOn()
{
    State st = state();

    if (st.st == State::BLINKING)
        return st.value;
    if (st.st == State::OFF)
        return false;
    if (st.st == State::ON)
        return true;
    ASSERT_LOG(0, "Unkown LED state. Aborting.");
    exit(-1);
}

bool FLED::isOff()
{
    return !isOn();
}

bool FLED::isBlinking()
{
    return state().st == State::BLINKING;
}

FLED::State FLED::state()
{
    FLED::State led_state;

    zmqpp::message rep;
    std::string status_str;

    backend_.send("STATE");
    backend_.receive(rep);

    rep >> status_str;
    if (status_str == "BLINKING")
    {
        assert(rep.parts() == 4);
        led_state.st = State::BLINKING;

        // duration
        rep >> led_state.duration;

        // speed
        rep >> led_state.speed;

        // value
        rep >> status_str;
        assert(status_str == "ON" || status_str == "OFF");
        led_state.value = status_str == "ON";
    }
    else
    {
        assert(rep.parts() == 1);
        if (status_str == "ON")
            led_state.st = State::ON;
        else if (status_str == "OFF")
            led_state.st = State::OFF;
        else
            assert(0);
    }
    return led_state;
}

zmqpp::socket &FLED::backend()
{
    return backend_;
}
