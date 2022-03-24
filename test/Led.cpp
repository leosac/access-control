/*
    Copyright (C) 2014-2022 Leosac

    This file is part of Leosac.

    Leosac is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leosac is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "LeosacFwd.hpp"
#include "hardware/facades/FLED.hpp"
#include "helper/TestHelper.hpp"
#include "modules/led-buzzer/LEDBuzzerModule.hpp"
#include "tools/runtimeoptions.hpp"
#include <string>

using namespace Leosac::Module::LedBuzzer;
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

        cfg.add("name", "LED_BUZZER");
        cfg.add_child("module_config", module_cfg);

        return test_run_module<LEDBuzzerModule>(&ctx_, pipe, cfg);
    }

  public:
    LedTest()
        : TestHelper()
        , gpio_(ctx_, "my_gpio")
        , gpio_actor_(std::bind(&FakeGPIO::run, &gpio_, std::placeholders::_1))
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

/**
* Test blinking, starting with a led that is OFF
*/
TEST_F(LedTest, blink1)
{
    FLED my_led(ctx_, "my_led");
    ASSERT_TRUE(my_led.isOff());
    ASSERT_TRUE(my_led.state().st == FLED::State::OFF);
    ASSERT_FALSE(my_led.isBlinking());

    my_led.blink(10, 1);
    ASSERT_TRUE(my_led.isBlinking());
    ASSERT_TRUE(my_led.state().st == FLED::State::BLINKING);
    // we should see 10 changes
    for (int i = 0; i < 5; ++i)
    {
        ASSERT_TRUE(bus_read(bus_sub_, "S_my_gpio", "ON"));
        ASSERT_TRUE(bus_read(bus_sub_, "S_my_gpio", "OFF"));
    }
    ASSERT_TRUE(my_led.isOff());
    ASSERT_EQ(FLED::State::OFF, my_led.state().st);
}

/**
* Test blinking, starting with a led that is ON.
* We blink 9 times, so the led will end up OFF.
*/
TEST_F(LedTest, blink2)
{
    FLED my_led(ctx_, "my_led");
    my_led.turnOn();
    ASSERT_TRUE(bus_read(bus_sub_, "S_my_gpio", "ON"));
    ASSERT_TRUE(my_led.isOn());
    ASSERT_TRUE(my_led.state().st == FLED::State::ON);
    ASSERT_FALSE(my_led.isBlinking());

    my_led.blink(18, 2);
    ASSERT_TRUE(my_led.isBlinking());
    ASSERT_TRUE(my_led.state().st == FLED::State::BLINKING);
    // we should see 9 changes
    for (int i = 0; i < 4; ++i)
    {
        ASSERT_TRUE(bus_read(bus_sub_, "S_my_gpio", "OFF"));
        ASSERT_TRUE(bus_read(bus_sub_, "S_my_gpio", "ON"));
    }
    ASSERT_TRUE(bus_read(bus_sub_, "S_my_gpio", "OFF"));
    ASSERT_TRUE(my_led.isOff());
    ASSERT_EQ(FLED::State::OFF, my_led.state().st);
}

/**
 * Test regression for #61
 */
TEST_F(LedTest, blink3)
{
    FLED my_led(ctx_, "my_led");
    ASSERT_TRUE(my_led.isOff());
    ASSERT_TRUE(my_led.state().st == FLED::State::OFF);
    ASSERT_FALSE(my_led.isBlinking());

    my_led.blink(100, 10);
    ASSERT_TRUE(my_led.isBlinking());
    ASSERT_TRUE(my_led.state().st == FLED::State::BLINKING);

    ASSERT_EQ(my_led.state().duration, 100);
    ASSERT_EQ(my_led.state().speed, 10);
}
}
}
