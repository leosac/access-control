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

bool run_module(zmqpp::context *ctx, zmqpp::socket *pipe)
    {
        boost::property_tree::ptree cfg, module_cfg, leds_cfg, led1_cfg;

        led1_cfg.add("name", "a_nice_led");
        led1_cfg.add("gpio", "a_cool_gpio");
        led1_cfg.add("default_blink_duration", "1000");
        led1_cfg.add("default_blink_speed", "100");

        leds_cfg.add_child("led", led1_cfg);
        module_cfg.add_child("leds", leds_cfg);
        cfg.add_child("module_config", module_cfg);

        return test_run_module<LEDModule>(ctx, pipe, cfg);
    }

TEST_F(LedTest, turnLedOn)
{
// create a facade
FLED my_led(ctx_, "my_led");

ASSERT_TRUE(my_led.turnOn());
ASSERT_TRUE(bus_read(bus_sub_, "S_my_gpio", "ON"));
}

/**
* Test that we can turn a led ON, and that it will tell the GPIO to turn ON.
*/
TEST(Led, turnOn)
{
    zmqpp::context ctx;
    MessageBus bus(ctx);
    zmqpp::socket bus_sub(ctx, zmqpp::socket_type::sub);

FakeGPIO gpio(ctx, "a_cool_gpio");
zmqpp::actor my_gpio_actor(std::bind(&FakeGPIO::run, &gpio, std::placeholders::_1));

    bus_sub.connect("inproc://zmq-bus-pub");
    bus_sub.subscribe("S_a_cool_gpio");
    zmqpp::actor a(std::bind(&run_module, &ctx, std::placeholders::_1));

FLED my_led(ctx, "a_nice_led");
ASSERT_TRUE(my_led.turnOn());
ASSERT_TRUE(bus_read(bus_sub, std::string("S_a_cool_gpio"), std::string("ON")));

ASSERT_TRUE(my_gpio_actor.stop(true));
}

TEST(Led, turnOff)
{
zmqpp::context ctx;
MessageBus bus(ctx);
zmqpp::socket bus_sub(ctx, zmqpp::socket_type::sub);

FakeGPIO gpio(ctx, "a_cool_gpio");
zmqpp::actor my_gpio_actor(std::bind(&FakeGPIO::run, &gpio, std::placeholders::_1));

bus_sub.connect("inproc://zmq-bus-pub");
bus_sub.subscribe("S_a_cool_gpio");
zmqpp::actor a(std::bind(&run_module, &ctx, std::placeholders::_1));

FLED my_led(ctx, "a_nice_led");
ASSERT_TRUE(my_led.turnOff());
ASSERT_TRUE(bus_read(bus_sub, "S_a_cool_gpio", "OFF"));

ASSERT_TRUE(my_gpio_actor.stop(true));
}

TEST(Led, toggle)
{
  zmqpp::context ctx;
    MessageBus bus(ctx);
    zmqpp::socket bus_sub(ctx, zmqpp::socket_type::sub);

FakeGPIO gpio(ctx, "a_cool_gpio");
zmqpp::actor my_gpio_actor(std::bind(&FakeGPIO::run, &gpio, std::placeholders::_1));

    bus_sub.connect("inproc://zmq-bus-pub");
    bus_sub.subscribe("S_a_cool_gpio");
    zmqpp::actor a(std::bind(&run_module, &ctx, std::placeholders::_1));

FLED my_led(ctx, "a_nice_led");
ASSERT_TRUE(my_led.toggle());
ASSERT_TRUE(bus_read(bus_sub, "S_a_cool_gpio", "ON"));
ASSERT_TRUE(my_led.toggle());
ASSERT_TRUE(bus_read(bus_sub, "S_a_cool_gpio", "OFF"));

ASSERT_TRUE(my_gpio_actor.stop(true));
}