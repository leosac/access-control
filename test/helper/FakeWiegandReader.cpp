/*
    Copyright (C) 2014-2016 Islog

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

#include "FakeWiegandReader.hpp"

using namespace Leosac::Test::Helper;

FakeWiegandReader::FakeWiegandReader(zmqpp::context &ctx, const std::string &name)
    : rep_(ctx, zmqpp::socket_type::rep)
{
    rep_.bind("inproc://" + name);
}

bool FakeWiegandReader::run(zmqpp::socket *pipe)
{
    zmqpp::poller poller;

    poller.add(*pipe);
    poller.add(rep_);

    pipe->send(zmqpp::signal::ok);

    while (true)
    {
        poller.poll(-1);

        if (poller.has_input(*pipe))
            break;

        if (poller.has_input(rep_))
        {
            zmqpp::message msg;
            rep_.receive(msg);
            rep_.send("OK");
        }
    }
    return true;
}