#include <tools/log.hpp>
#include "TestAndResetModule.hpp"

TestAndResetModule::~TestAndResetModule()
{

}

TestAndResetModule::TestAndResetModule(zmqpp::context &ctx,
        zmqpp::socket *pipe,
        const boost::property_tree::ptree &cfg) :
        BaseModule(ctx, pipe, cfg),
        kernel_sock_(ctx, zmqpp::socket_type::req),
        sub_(ctx, zmqpp::socket_type::sub)

{
    sub_.connect("inproc://zmq-bus-pub");
    kernel_sock_.connect("inproc://leosac-kernel");

    process_config();
    reactor_.add(sub_, std::bind(&TestAndResetModule::handle_bus_msg, this));
}

void TestAndResetModule::process_config()
{
    boost::property_tree::ptree module_config = config_.get_child("module_config");

    for (auto &node : module_config.get_child("devices"))
    {
        boost::property_tree::ptree device_cfg = node.second;

        std::string device_name = device_cfg.get_child("name").data();
        std::string reset_card = device_cfg.get_child("reset_card").data();

        sub_.subscribe("S_" + device_name);
        device_reset_card_[device_name] = reset_card;
    }
}

void TestAndResetModule::handle_bus_msg()
{
    zmqpp::message msg;
    std::string src;
    std::string card;

    sub_.receive(msg);

    assert(msg.parts() >= 2);
    msg >> src >> card;
    // remove "S_" from topic string
    src = src.substr(2, src.size());
    if (device_reset_card_.count(src) && device_reset_card_[src] == card)
    {
        kernel_sock_.send("RESET");
    }
}
