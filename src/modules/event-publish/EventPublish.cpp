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
    bus_sub_.subscribe("S_MY_WIEGAND_1");

    network_pub_.bind("tcp://*:4243");

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
    network_pub_.send(card);
}

void EventPublish::process_config()
{

}
