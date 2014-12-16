#include <tools/log.hpp>
#include "RemoteControl.hpp"
#include "kernel.hpp"

using namespace Leosac;

RemoteControl::RemoteControl(zmqpp::context &ctx,
        Kernel &kernel,
        const boost::property_tree::ptree &cfg) :
        kernel_(kernel),
        socket_(ctx, zmqpp::socket_type::router)
{
    process_config(cfg);
}

void RemoteControl::process_config(const boost::property_tree::ptree &cfg)
{
    int port = cfg.get<int>("port");

    INFO("Binding RemoteControl socket to port " << port);
    socket_.bind("tcp://*:" + std::to_string(port));
}

void RemoteControl::handle_msg()
{
    zmqpp::message msg;
    std::string source;
    socket_.receive(msg);

    assert(msg.parts() > 1);
    msg >> source;
    INFO("New message from {" << std::hex << source << std::resetiosflags << "}");
}
