/*
    Copyright (C) 2014-2016 Islog

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

#include "core/Scheduler.hpp"
#include "core/auth/Auth.hpp"
#include "helper/TestHelper.hpp"
#include "modules/wiegand/wiegand.hpp"
#include "tools/runtimeoptions.hpp"

using namespace Leosac::Module::Wiegand;
using namespace Leosac::Test::Helper;

namespace Leosac
{
namespace Test
{
class WiegandReaderTest : public Helper::TestHelper
{
  private:
    virtual bool run_module(zmqpp::socket *pipe) override
    {
        boost::property_tree::ptree cfg, module_cfg, readers_cfg, reader1_cfg;

        reader1_cfg.add("name", "WIEGAND_1");
        reader1_cfg.add("high", "GPIO_HIGH");
        reader1_cfg.add("low", "GPIO_LOW");

        readers_cfg.add_child("reader", reader1_cfg);
        module_cfg.add_child("readers", readers_cfg);

        cfg.add("name", "WIEGAND_READER");
        cfg.add_child("module_config", module_cfg);

        return test_run_module<WiegandReaderModule>(&ctx_, pipe, cfg);
    }

  public:
    WiegandReaderTest()
        : TestHelper()
        , high_(ctx_, "GPIO_HIGH")
        , low_(ctx_, "GPIO_LOW")
    {
        bus_sub_.subscribe("S_WIEGAND_1");
    }

    ~WiegandReaderTest()
    {
    }

    FakeGPIO high_;
    FakeGPIO low_;
};

TEST_F(WiegandReaderTest, readCard)
{
    for (int i = 0; i < 32; i++)
    {
        high_.interrupt(); // building card id ff:ff:ff:ff
    }

    ASSERT_TRUE(bus_read(bus_sub_, "S_WIEGAND_1",
                         Leosac::Auth::SourceType::SIMPLE_WIEGAND, "ff:ff:ff:ff",
                         32));

    for (int i = 0; i < 32; i++)
    {
        if (i >= 24)
            high_.interrupt();
        else
            low_.interrupt();

        // required because zmq sockets do fair-queuing.
        // its not a problem either, because the hardware will pause too.
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    ASSERT_TRUE(bus_read(bus_sub_, "S_WIEGAND_1",
                         Leosac::Auth::SourceType::SIMPLE_WIEGAND, "00:00:00:ff",
                         32));
}
}
}
