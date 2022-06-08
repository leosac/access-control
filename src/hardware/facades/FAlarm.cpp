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

#include "FAlarm.hpp"

using namespace Leosac::Hardware;

FAlarm::FAlarm(zmqpp::context &ctx, std::string const &name)
    : backend_(ctx, zmqpp::socket_type::req)
    , name_(name)
{
    backend_.connect("inproc://" + name);
}

std::string FAlarm::raise(AlarmType type, const std::string& reason)
{
    zmqpp::message msg;
    msg << "RAISE" << static_cast<uint8_t>(type) << reason;
    auto rep = send_to_backend(msg);
    std::string r, alarm;
    rep >> r;
    if (r == "OK" && rep.remaining() == 1)
    {
      rep >> alarm;
    }

    return alarm;
}

AlarmState FAlarm::state(const std::string& alarm)
{
    zmqpp::message msg;
    msg << "GET_STATE" << alarm;
    auto rep = send_to_backend(msg);
    uint8_t state = static_cast<uint8_t>(AlarmState::STATE_UNKNOWN);
    std::string r;
    rep >> r;
    if (r == "OK" && rep.remaining() == 1)
    {
      rep >> state;
    }

    return static_cast<AlarmState>(state);
}

bool FAlarm::state(const std::string& alarm, AlarmState state)
{
    zmqpp::message msg;
    msg << "SET_STATE" << alarm << static_cast<uint8_t>(state);
    auto rep = send_to_backend(msg);
    std::string r;
    rep >> r;
    return (r == "OK");
}

bool FAlarm::disarm(const std::string& alarm)
{
    zmqpp::message msg;
    msg << "DISARM" << alarm;
    auto rep = send_to_backend(msg);
    std::string r;
    rep >> r;

    return (r == "OK");
}

zmqpp::message_t FAlarm::send_to_backend(zmqpp::message &msg)
{
    zmqpp::message_t rep;
    backend_.send(msg);
    backend_.receive(rep);
    return rep;
}

const std::string &FAlarm::name() const
{
    return name_;
}
