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

#include <core/auth/Auth.hpp>
#include "EventPublish.h"

using namespace Leosac::Module::EventPublish;

EventPublish::EventPublish(zmqpp::context &ctx, zmqpp::socket *pipe,
                           const boost::property_tree::ptree &cfg,
                           CoreUtilsPtr utils)
    : BaseModule(ctx, pipe, cfg, utils)
    , bus_sub_(ctx, zmqpp::socket_type::sub)
    , network_pub_(ctx, zmqpp::socket_type::pub)
{
    bus_sub_.connect("inproc://zmq-bus-pub");
    process_config();
    reactor_.add(bus_sub_, std::bind(&EventPublish::handle_msg_bus, this));
}

void EventPublish::handle_msg_bus()
{
    zmqpp::message msg;
    std::string src;
    Leosac::Auth::SourceType type;
    std::string card;
    int bits;

    bus_sub_.receive(msg);

    if (msg.parts() < 4)
        return;
    msg >> src >> type >> card >> bits;
    INFO("Will publish card id {" << card << "}");

    if (type != Leosac::Auth::SourceType::SIMPLE_WIEGAND)
    {
        INFO("EventPublish cannot handle this type of credential yet.");
        return;
    }

    // Hack Alert! Hack Alert!
    // For this use case, we read 35bits wiegand frame CP1000 encoded.
    // We want to extract the identifier: the 20 second-to-last bits (14 first bit
    // ignored).
    // In case this is not a 35 bits tram, do nothing
    if (bits == 35)
    {
        std::stringstream ss;
        ss << std::hex << "0x";
        for (auto c : card)
        {
            if (c == ':')
                continue;
            ss << std::hex << c;
        }
        INFO("Bla: {" << ss.str() << "}");
        // n has 40 "meaningful" bits because the hex string was 5 bytes.
        uint64_t n;
        ss >> n;
        INFO(" n = " << n);
        n = (n >> 6); // remove last 6bits. parity bit + 5bits;
        n &= 0xFFFFF; // keep 20 bits.
        INFO(" n2 = " << n);
        std::stringstream ss2;
        ss2 << n;
        ss2 >> card;
    }
    else
        return;

    if (publish_source_)
        network_pub_.send(zmqpp::message() << card << src);
    else
        network_pub_.send(card);
}

void EventPublish::process_config()
{
    auto port = config_.get<int>("module_config.port");
    network_pub_.bind("tcp://*:" + std::to_string(port));
    publish_source_ = config_.get<bool>("module_config.publish_source", false);

    for (auto &&itr : config_.get_child("module_config.sources"))
    {
        auto name = itr.second.get<std::string>("");
        bus_sub_.subscribe("S_" + name);
    }
}
