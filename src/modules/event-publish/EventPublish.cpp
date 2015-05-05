//
// Created by xaqq on 3/31/15.
//

#include <core/auth/Auth.hpp>
#include "EventPublish.h"

using namespace Leosac::Module::EventPublish;

EventPublish::EventPublish(zmqpp::context &ctx, zmqpp::socket *pipe,
                           const boost::property_tree::ptree &cfg,
                           Scheduler &sched) :
        BaseModule(ctx, pipe, cfg, sched),
        bus_sub_(ctx, zmqpp::socket_type::sub),
        network_pub_(ctx, zmqpp::socket_type::pub)
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

    INFO("Publishing card id {" << card << "}");
    bus_sub_.receive(msg);

    if (msg.parts() < 4)
        return;
    msg >> src >> type >> card >> bits;

    if (type != Leosac::Auth::SourceType::SIMPLE_WIEGAND)
    {
        INFO("EventPublish cannot handle this type of credential yet.");
        return;
    }

    // Hack Alert! Hack Alert!
    // For this use case, we read 35bits wiegand frame CP1000 encoded.
    // We want to extract the identifier: the 20 second-to-last bits (14 first bit ignored).
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

    for (auto && itr : config_.get_child("module_config.sources"))
    {
        auto name = itr.second.get<std::string>("");
        bus_sub_.subscribe("S_" + name);
    }
}
