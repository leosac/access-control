#include <tools/runtimeoptions.hpp>
#include <modules/wiegand/wiegand.hpp>
#include "helper/TestHelper.hpp"

bool run_module(zmqpp::context *ctx, zmqpp::socket *pipe)
    {
    boost::property_tree::ptree cfg, module_cfg, readers_cfg, reader1_cfg;

    reader1_cfg.add("name", "WIEGAND_1");
    reader1_cfg.add("high", "GPIO_HIGH");
    reader1_cfg.add("low", "GPIO_LOW");

    readers_cfg.add_child("reader", reader1_cfg);
    module_cfg.add_child("readers", readers_cfg);
    cfg.add_child("module_config", module_cfg);

        return test_run_module<WiegandReaderModule>(ctx, pipe, cfg);
    }

TEST(WiegandReader, readCard)
{
    zmqpp::context ctx;
    MessageBus bus(ctx);
zmqpp::socket bus_sub(ctx, zmqpp::socket_type::sub);

    std::vector<std::shared_ptr<FakeGPIO>> gpios;

    gpios.push_back(std::shared_ptr<FakeGPIO> (new FakeGPIO(ctx, "GPIO_HIGH")));
    gpios.push_back(std::shared_ptr<FakeGPIO> (new FakeGPIO(ctx, "GPIO_LOW")));

    bus_sub.connect("inproc://zmq-bus-pub");
    bus_sub.subscribe("S_WIEGAND_1");

    zmqpp::actor a(std::bind(&run_module, &ctx, std::placeholders::_1));

    for (int i = 0 ; i < 32; i++)
        {
gpios[0]->interrupt(); // building card id ff:ff:ff:ff
        }

ASSERT_TRUE(bus_read(bus_sub, "S_WIEGAND_1", "ff:ff:ff:ff"));

    for (int i = 0 ; i < 32; i++)
        {
if (i >= 24)
{
gpios[0]->interrupt();
}
else
{
gpios[1]->interrupt();

//required because zmq sockets do fair-queuing.
// its not a problem either, because the hardware will pause too.
std::this_thread::sleep_for(std::chrono::milliseconds(2));
}
        }

ASSERT_TRUE(bus_read(bus_sub, "S_WIEGAND_1", "ff:00:00:00"));
    ASSERT_TRUE(a.stop(true));
    }
