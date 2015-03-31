//
// Created by xaqq on 3/31/15.
//

#include <core/auth/Auth.hpp>
#include "EventPublish.h"

using namespace Leosac::Module::EventPublish;

EventPublish::EventPublish(zmqpp::context &ctx, zmqpp::socket *pipe,
                           const boost::property_tree::ptree &cfg) :
        BaseModule(ctx, pipe, cfg),
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

    INFO("Publishing card id {" << card << "}");
    bus_sub_.receive(msg);

    if (msg.parts() < 4)
        return;
    msg >> src >> type >> card;

    if (type != Leosac::Auth::SourceType::SIMPLE_WIEGAND)
    {
        INFO("EventPublish cannot handle this type of credential yet.");
        return;
    }

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
