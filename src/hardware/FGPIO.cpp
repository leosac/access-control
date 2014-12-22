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

#include "FGPIO.hpp"
#include <zmqpp/message.hpp>

using namespace Leosac::Hardware;

FGPIO::FGPIO(zmqpp::context &ctx, const std::string &gpio_name) :
        backend_(ctx, zmqpp::socket_type::req)
{
    backend_.connect("inproc://" + gpio_name);
}

bool FGPIO::turnOn(std::chrono::milliseconds duration)
{
    std::string rep;
    zmqpp::message msg;

    msg << "ON" << duration.count();

    backend_.send(msg);
    backend_.receive(rep);
    if (rep == "OK")
        return true;
    return false;
}

bool FGPIO::turnOn()
{
    std::string rep;

    backend_.send("ON");
    backend_.receive(rep);
    if (rep == "OK")
        return true;
    return false;
}

bool FGPIO::turnOff()
{
    std::string rep;

    backend_.send("OFF");
    backend_.receive(rep);
    if (rep == "OK")
        return true;
    return false;
}

bool FGPIO::toggle()
{
    std::string rep;

    backend_.send("TOGGLE");
    backend_.receive(rep);
    if (rep == "OK")
        return true;
    return false;
}

bool FGPIO::isOn()
{
    std::string rep;

    backend_.send("STATE");
    backend_.receive(rep);

    if (rep == "ON")
        return true;
    assert(rep == "OFF");
    return false;
}

bool FGPIO::isOff()
{
    return !isOn();
}
