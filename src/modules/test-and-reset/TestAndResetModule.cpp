/*
    Copyright (C) 2014-2015 Islog

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

#include <tools/log.hpp>
#include "core/auth/Auth.hpp"
#include "TestAndResetModule.hpp"

using namespace Leosac::Module::TestAndReset;
using namespace Leosac::Hardware;

TestAndResetModule::~TestAndResetModule()
{
}

TestAndResetModule::TestAndResetModule(zmqpp::context &ctx,
        zmqpp::socket *pipe,
        const boost::property_tree::ptree &cfg) :
        BaseModule(ctx, pipe, cfg),
        kernel_sock_(ctx, zmqpp::socket_type::req),
        sub_(ctx, zmqpp::socket_type::sub),
        test_led_(nullptr),
        test_buzzer_(nullptr),
        run_on_start_(true),
        promisc_(false)
{
    sub_.connect("inproc://zmq-bus-pub");
    kernel_sock_.connect("inproc://leosac-kernel");

    process_config();
    reactor_.add(sub_, std::bind(&TestAndResetModule::handle_bus_msg, this));
    if (run_on_start_)
        run_test_sequence();
}

void TestAndResetModule::process_config()
{
    boost::property_tree::ptree module_config   = config_.get_child("module_config");
    std::string test_device_led                 = module_config.get<std::string>("test_led", "");
    std::string test_device_buzzer              = module_config.get<std::string>("test_buzzer", "");
    run_on_start_                               = module_config.get<bool>("run_on_start", true);
    promisc_                                    = module_config.get<bool>("promisc", false);
    const auto &devices                         = module_config.get_child_optional("devices");

    if (!test_device_led.empty())
        test_led_ = std::unique_ptr<FLED>(new FLED(ctx_, test_device_led));
    if (!test_device_buzzer.empty())
        test_buzzer_ = std::unique_ptr<FLED>(new FLED(ctx_, test_device_buzzer));

    if (devices)
    {
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

    if (promisc_)
    {
        std::string reset_card = module_config.get<std::string>("reset_card");
        device_reset_card_["__promisc"] = reset_card;
        sub_.subscribe("");
    }
}

void TestAndResetModule::handle_bus_msg()
{
    zmqpp::message msg;
    std::string src;
    Leosac::Auth::SourceType type;
    std::string card;

    sub_.receive(msg);

    if (msg.parts() < 4)
        return;
    msg >> src >> type >> card;

    if (type != Leosac::Auth::SourceType::SIMPLE_WIEGAND)
    {
        ERROR("Invalid auth source type ! Doing nothing.");
        return;
    }

    if (promisc_)
    {
        if (has_reset_card(card))
            kernel_sock_.send("RESET");
    }
    else
    {
        // remove "S_" from topic string
        src = src.substr(2, src.size());
        if (device_reset_card_.count(src) && device_reset_card_[src] == card)
        {
            kernel_sock_.send("RESET");
        }
    }

    if (device_test_card_.count(src) && device_test_card_[src] == card)
    {
        INFO("Test card read.");
        run_test_sequence();
    }
}

void TestAndResetModule::run_test_sequence()
{
    DEBUG("Running test sequence...");
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
        NOTICE("Test sequence doing nothing...");
    }
}

bool TestAndResetModule::has_reset_card(const std::string &card_id) const
{
    return std::find_if(device_reset_card_.begin(),
            device_reset_card_.end(),
            [&](std::pair<std::string, std::string> p) -> bool {
                return p.second == card_id;
            }) != device_reset_card_.end();
}
