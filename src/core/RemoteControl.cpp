#include <tools/log.hpp>
#include <zmqpp/curve.hpp>
#include "RemoteControl.hpp"
#include "kernel.hpp"

using namespace Leosac;

RemoteControl::RemoteControl(zmqpp::context &ctx,
        Kernel &kernel,
        const boost::property_tree::ptree &cfg) :
        kernel_(kernel),
        socket_(ctx, zmqpp::socket_type::router),
        auth_(ctx)
{
    auth_.set_verbose(true);
    auth_.configure_curve("CURVE_ALLOW_ANY");
    process_config(cfg);
}

void RemoteControl::process_config(const boost::property_tree::ptree &cfg)
{
    int port = cfg.get<int>("port");

    secret_key_ = cfg.get<std::string>("secret_key");
    public_key_ = cfg.get<std::string>("public_key");

    INFO("Binding RemoteControl socket to port " << port);
    INFO("Use private key {" << secret_key_ << "} and public key {" << public_key_ << "}");

    socket_.set(zmqpp::socket_option::curve_server, true);
    socket_.set(zmqpp::socket_option::curve_secret_key, secret_key_);
    socket_.set(zmqpp::socket_option::curve_public_key, public_key_);
    socket_.bind("tcp://*:" + std::to_string(port));
}

void RemoteControl::handle_msg()
{
    zmqpp::message msg;
    zmqpp::message rep;
    std::string source;
    std::string frame1;
    socket_.receive(msg);

    assert(msg.parts() > 1);
    DEBUG("PARTS = " << msg.parts());
    msg >> source;
    if (std::find(test_.begin(), test_.end(), source) != test_.end())
        DEBUG("FOUND !!!");
    else
        test_.push_back(source);
    rep << source;
    std::stringstream ss;
    for (auto c : source)
        ss << std::hex << (int)c;
    INFO("(" << test_.size() << ") New message from {" << ss.str() << "}");

    msg >> frame1;
    DEBUG("Cmd = {" << frame1 << "}");
    if (frame1 == "MODULE_LIST")
        module_list(&rep);

    DEBUG("Sending response, " << rep.parts() << " frames");
    socket_.send(rep);
}

void RemoteControl::module_list(zmqpp::message *message_out)
{
    assert(message_out);
    DEBUG("HERE");
    for (const std::string &s : kernel_.module_manager().modules_names())
    {
        DEBUG("Module {" << s << "}");
        *message_out << s;
    }
}
