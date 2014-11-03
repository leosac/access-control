#include <tools/log.hpp>
#include "TestAndResetModule.hpp"

using namespace Leosac::Module::TestAndReset;

TestAndResetModule::~TestAndResetModule()
{
    delete test_led_;
    delete test_buzzer_;
}

TestAndResetModule::TestAndResetModule(zmqpp::context &ctx,
        zmqpp::socket *pipe,
        const boost::property_tree::ptree &cfg) :
        BaseModule(ctx, pipe, cfg),
        kernel_sock_(ctx, zmqpp::socket_type::req),
        sub_(ctx, zmqpp::socket_type::sub),
        test_led_(nullptr),
        test_buzzer_(nullptr)

{
    sub_.connect("inproc://zmq-bus-pub");
    kernel_sock_.connect("inproc://leosac-kernel");

    process_config();
    reactor_.add(sub_, std::bind(&TestAndResetModule::handle_bus_msg, this));
}

void TestAndResetModule::process_config()
{
    boost::property_tree::ptree module_config = config_.get_child("module_config");

    std::string test_device_led = module_config.get<std::string>("test_led", "");
    std::string test_device_buzzer = module_config.get<std::string>("test_buzzer", "");

    if (!test_device_led.empty())
        test_led_ = new FLED(ctx_, test_device_led);
    if (!test_device_buzzer.empty())
        test_buzzer_ = new FLED(ctx_, test_device_buzzer);

    for (auto &node : module_config.get_child("devices"))
    {
        boost::property_tree::ptree device_cfg = node.second;

        std::string device_name = device_cfg.get_child("name").data();
        std::string reset_card = device_cfg.get<std::string>("reset_card", "");
        std::string test_card = device_cfg.get<std::string>("test_card", "");

        sub_.subscribe("S_" + device_name);
        if (!reset_card.empty())
            device_reset_card_[device_name] = reset_card;
        if (!test_card.empty())
            device_test_card_[device_name] = test_card;
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
    if (device_test_card_.count(src) && device_test_card_[src] == card)
    {
        test_sequence();
    }
}

void TestAndResetModule::test_sequence()
{
    INFO("Test card read.");
    if (test_buzzer_)
    {
        test_buzzer_->blink(4000, 500);
    }
    if (test_led_)
    {
        test_led_->blink(4000, 500);
    }
    if (!test_led_ && !test_buzzer_)
    {
        INFO("Test card read, but doing nothing");
    }

}
