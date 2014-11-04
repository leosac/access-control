#include <string>
#include "modules/led/LEDModule.hpp"
#include "helper/TestHelper.hpp"
#include "hardware/FLED.hpp"
#include "tools/runtimeoptions.hpp"

using namespace Leosac::Module::Led;
using namespace Leosac::Test::Helper;
using namespace Leosac::Hardware;

namespace Leosac
{
    namespace Test
    {
        class LedTest : public Helper::TestHelper
        {
        private:
            // starts the led module
            virtual bool run_module(zmqpp::socket *pipe) override
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
                    TestHelper(),
                    gpio_(ctx_, "my_gpio"),
                    gpio_actor_(std::bind(&FakeGPIO::run, &gpio_, std::placeholders::_1))
            {
                bus_sub_.subscribe("");
            }

            ~LedTest()
            {
            }

            FakeGPIO gpio_;

            // to allow the fake gpio to react to command.
            zmqpp::actor gpio_actor_;
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

            ASSERT_TRUE(my_led.turnOn());
            ASSERT_TRUE(my_led.isOn());
            ASSERT_TRUE(bus_read(bus_sub_, "S_my_gpio", "ON"));

            ASSERT_TRUE(my_led.turnOff());
            ASSERT_TRUE(bus_read(bus_sub_, "S_my_gpio", "OFF"));
            ASSERT_TRUE(my_led.isOff());
        }

        TEST_F(LedTest, toggle)
        {
            FLED my_led(ctx_, "my_led");
            ASSERT_TRUE(my_led.isOff());

            // fake gpio start off
            ASSERT_TRUE(my_led.toggle());
            ASSERT_TRUE(bus_read(bus_sub_, "S_my_gpio", "ON"));
            ASSERT_TRUE(my_led.isOn());

            ASSERT_TRUE(my_led.toggle());
            ASSERT_TRUE(bus_read(bus_sub_, "S_my_gpio", "OFF"));
            ASSERT_TRUE(my_led.isOff());
        }

        TEST_F(LedTest, blink)
        {
            FLED my_led(ctx_, "my_led");
            ASSERT_TRUE(my_led.isOff());
            ASSERT_TRUE(my_led.state().st == FLED::State::OFF);
            ASSERT_FALSE(my_led.isBlinking());

            my_led.blink(1000, 100);
            ASSERT_TRUE(my_led.isBlinking());
            ASSERT_TRUE(my_led.state().st == FLED::State::BLINKING);
            // we should see 10 changes
            for (int i = 0; i < 5; ++i)
            {
                ASSERT_TRUE(bus_read(bus_sub_, "S_my_gpio", "ON"));
                ASSERT_TRUE(my_led.isBlinking());
                ASSERT_TRUE(bus_read(bus_sub_, "S_my_gpio", "OFF"));
            }

            // the "final" off: sent when blinking is over (even if gpio is off already)
            ASSERT_TRUE(bus_read(bus_sub_, "S_my_gpio", "OFF"));
            ASSERT_TRUE(my_led.isOff());

            ASSERT_FALSE(my_led.isBlinking());
            ASSERT_TRUE(my_led.isOff());
            ASSERT_TRUE(my_led.state().st == FLED::State::OFF);
        }
    }
}
