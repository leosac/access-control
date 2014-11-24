#include <core/auth/Auth.hpp>
#include "DoormanInstance.hpp"
#include "tools/log.hpp"

using namespace Leosac::Module::Doorman;

DoormanInstance::DoormanInstance(zmqpp::context &ctx,
        std::string const &name,
        const std::vector<std::string> &auth_contexts,
        const std::vector<DoormanAction> &actions,
        int timeout) :
        name_(name),
        actions_(actions),
        timeout_(timeout),
        bus_sub_(ctx, zmqpp::socket_type::sub)
{
    bus_sub_.connect("inproc://zmq-bus-pub");
    for (auto &endpoint : auth_contexts)
        bus_sub_.subscribe("S_" + endpoint);

    for (auto &action : actions_)
    {
        if (targets_.count(action.target_))
            continue; // already have a socket to this target.

        // create socket (and connect them) to target
        zmqpp::socket target_socket(ctx, zmqpp::socket_type::req);
        target_socket.connect("inproc://" + action.target_);
        targets_.insert(std::make_pair(action.target_, std::move(target_socket)));
    }
}

zmqpp::socket &DoormanInstance::bus_sub()
{
    return bus_sub_;
}

void DoormanInstance::handle_bus_msg()
{
    zmqpp::message bus_msg;
    std::string auth_name; // name of the auth context that sent the message.
    Leosac::Auth::AccessStatus access_status;

    bus_sub_.receive(bus_msg);
    assert(bus_msg.parts() >= 2);
    bus_msg >> auth_name >> access_status;
    DEBUG("DOORMAN HERE");

    for (auto &action : actions_)
    {
        if (action.on_ != access_status)
            continue; // status doesn't match what we expected.
        DEBUG("ACTION (target = " << action.target_ << ")");
        zmqpp::message msg;
        for (auto &frame : action.cmd_)
        {
            msg << frame;
            DEBUG("would do : " << frame << " to target: " << action.target_);
        }
        command_send_recv(action.target_, std::move(msg));
    }
}

void DoormanInstance::command_send_recv(std::string const &target_name, zmqpp::message msg)
{
    zmqpp::socket &target_socket = targets_.at(target_name);
    zmqpp::message response;

    target_socket.send(msg);
    target_socket.receive(response);

    std::string req_status;
    response >> req_status;

    if (req_status != "OK")
    {
        WARN("Command failed :(");
    }
}
