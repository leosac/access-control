#include <tools/log.hpp>
#include "InstrumentationModule.hpp"

using namespace Leosac::Module::Instrumentation;

InstrumentationModule::InstrumentationModule(zmqpp::context &ctx,
        zmqpp::socket *pipe,
        const boost::property_tree::ptree &cfg) :
        BaseModule(ctx, pipe, cfg),
        bus_push_(ctx, zmqpp::socket_type::push),
        controller_(ctx, zmqpp::socket_type::router)
{
    std::string bind_str = "ipc://" + config_.get_child("module_config").get<std::string>("ipc_endpoint");
    controller_.bind(bind_str);
    INFO("Binding to: " << bind_str);
    bus_push_.connect("inproc://zmq-bus-push");
    reactor_.add(controller_, std::bind(&InstrumentationModule::handle_command, this));
}

void InstrumentationModule::handle_command()
{
    INFO("COMMAND AVAILABLE");
    zmqpp::message msg;

    controller_.receive(msg);
    std::string identity;
    std::string str;

    msg >> identity >> str;
    if (str == "GPIO")
    {
        handle_gpio_command(&msg);
    }
    else
    {
        NOTICE("Received unhandled command: " << str);
    }
}

void InstrumentationModule::handle_gpio_command(zmqpp::message *str)
{
    assert(str);
    std::string cmd;
    std::string gpio_name;

    *str >> gpio_name >> cmd;
    DEBUG("GPIO CMD: " << gpio_name << ", " << cmd);
    if (cmd == "ON")
    {
        bus_push_.send(zmqpp::message() << ("S_" + gpio_name) << "ON");
    }
    else if (cmd == "OFF")
    {
        bus_push_.send(zmqpp::message() << ("S_" + gpio_name) << "OFF");
    }
    else if (cmd == "INT")
    {
        bus_push_.send(zmqpp::message() << std::string("S_INT:" + gpio_name));
    }
}
