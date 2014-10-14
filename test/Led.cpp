#include <tools/runtimeoptions.hpp>
#include <string>
#include "modules/led/LEDModule.hpp"
#include "helper/TestHelper.hpp"
#include "hardware/device/FLED.hpp"


class LedTest : public ::testing::Test
{
private:
    // starts the led module
    bool run_module(zmqpp::socket *pipe)
    {
        boost::property_tree::ptree cfg, module_cfg, leds_cfg, led1_cfg;

        led1_cfg.add("name", "my_led");
        led1_cfg.add("gpio", "my_gpio");
        led1_cfg.add("default_blink_duration", "1000");
        led1_cfg.add("default_blink_speed", "100");

        leds_cfg.add_child("led", led1_cfg);
        module_cfg.add_child("leds", leds_cfg);
        cfg.add_child("module_config", module_cfg);

        return test_run_module<LEDModule>(&ctx_, pipe, cfg);
    }

public:

    LedTest() :
    ctx_
            (),
    bus_(ctx_),
    bus_sub_(ctx_, zmqpp::socket_type::sub),
    gpio_(ctx_, "my_gpio"),
    gpio_actor_(std::bind(&FakeGPIO::run, &gpio_, std::placeholders::_1)),
    module_actor_(std::bind(&LedTest::run_module, this, std::placeholders::_1))
{
    bus_sub_.connect("inproc://zmq-bus-pub");
    bus_sub_.subscribe("");
}

    ~LedTest()
    {
    }

    zmqpp::context ctx_;
    MessageBus bus_;
    // subscribe socket to the bus
    zmqpp::socket bus_sub_;

    FakeGPIO gpio_;
    // to allow the fake gpio to react to command.
    zmqpp::actor gpio_actor_;

    // run the module code (like it was loaded by core)
    zmqpp::actor module_actor_;
};

TEST_F(LedTest, turnOn)
{
// create a facade
FLED my_led(ctx_, "my_led");
ASSERT_TRUE(my_led.isOff());

ASSERT_TRUE(my_led.turnOn());
ASSERT_TRUE(my_led.isOn());
ASSERT_TRUE(bus_read(bus_sub_, "S_my_gpio", "ON"));
}

TEST_F(LedTest, turnOff)
{
FLED my_led(ctx_, "my_led");
ASSERT_TRUE(my_led.isOff());

ASSERT_TRUE(my_led.turnOff());
ASSERT_TRUE(bus_read(bus_sub_, "S_my_gpio", "OFF"));
}

TEST_F(LedTest, toggle)
{
FLED my_led(ctx_, "my_led");
ASSERT_TRUE(my_led.isOff());

// fake gpio start off
ASSERT_TRUE(my_led.toggle());
ASSERT_TRUE(bus_read(bus_sub_, "S_my_gpio", "ON"));

ASSERT_TRUE(my_led.toggle());
ASSERT_TRUE(bus_read(bus_sub_, "S_my_gpio", "OFF"));
}

TEST_F(LedTest, blink)
{
FLED my_led(ctx_, "my_led");

my_led.blink(100, 10);

// we should see 10 value change
for (int i = 0; i < 5; ++i)
{
ASSERT_TRUE(my_led.toggle());
ASSERT_TRUE(bus_read(bus_sub_, "S_my_gpio", "ON"));

ASSERT_TRUE(my_led.toggle());
ASSERT_TRUE(bus_read(bus_sub_, "S_my_gpio", "OFF"));
}

}