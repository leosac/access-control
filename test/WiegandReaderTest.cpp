#include <tools/runtimeoptions.hpp>
#include <modules/wiegand/wiegand.hpp>
#include <boost/property_tree/ptree.hpp>
#include "gtest/gtest.h"
#include "core/MessageBus.hpp"

bool run_module(zmqpp::context *ctx, zmqpp::socket *pipe)
    {
    boost::property_tree::ptree cfg, module_cfg, readers_cfg, reader1_cfg;

    reader1_cfg.add("name", "WIEGAND_1");
    reader1_cfg.add("high", "GPIO_HIGH");
    reader1_cfg.add("low", "GPIO_LOW");

    readers_cfg.add_child("reader", reader1_cfg);
    module_cfg.add_child("readers", readers_cfg);
    cfg.add_child("module_config", module_cfg);
    WiegandReaderModule module(*ctx, pipe, cfg);

    pipe->send(zmqpp::signal::ok);
    module.run();
    return true;
    }

class GPIOEmulation
    {
public:
    GPIOEmulation(zmqpp::socket &push, const std::string &n) :
    name_(n),
    push_(push)
        {};

    void interrupt()
        {
        push_.send("S_INT:" + name_);
        }

    std::string name_;
    zmqpp::socket &push_;
    };

TEST(WiegandReaderTest, readCard)
{
    zmqpp::context ctx;
    MessageBus bus(ctx);
    zmqpp::socket bus_push(ctx, zmqpp::socket_type::push);
    zmqpp::socket bus_sub(ctx, zmqpp::socket_type::sub);

    std::vector<GPIOEmulation> gpios;

    gpios.push_back({bus_push, "GPIO_HIGH"});
    gpios.push_back({bus_push, "GPIO_LOW"});
    bus_push.connect("inproc://zmq-bus-pull");

    bus_sub.connect("inproc://zmq-bus-pub");
    bus_sub.subscribe("S_WIEGAND_1");

    zmqpp::actor a(std::bind(&run_module, &ctx, std::placeholders::_1));

    for (int i = 0 ; i < 32; i++)
        {
        gpios[0].interrupt(); // building card id ff:ff:ff:ff
        }

    zmqpp::message m;
    bus_sub.receive(m);

    std::string topic, content;
    ASSERT_EQ(2, m.parts());
        m >> topic >> content;
    ASSERT_EQ("S_WIEGAND_1", topic);
    ASSERT_EQ("ff:ff:ff:ff", content);

    ASSERT_TRUE(a.stop(true));
    }
