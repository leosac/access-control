#include "DoormanInstance.hpp"
#include "tools/log.hpp"

DoormanInstance::DoormanInstance(zmqpp::context &ctx,
        std::string const &name,
        const std::vector <std::string> &auth_contexts,
        const std::vector <DoormanAction> &actions,
        int timeout) :
name_(name),
actions_(actions),
timeout_(timeout),
bus_sub_(ctx, zmqpp::socket_type::sub)
{
    bus_sub_.connect("inproc://zmq-bus-pub");
}

socket &DoormanInstance::bus_sub()
{
    return bus_sub_;
}

void DoormanInstance::handle_bus_msg()
{
    LOG() << "DOORMAN HERE";
}
