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

#include "LEDBuzzerModule.hpp"
#include "LedBuzzerImpl.hpp"
#include "tools/log.hpp"

using namespace Leosac::Module::LedBuzzer;

LEDBuzzerModule::LEDBuzzerModule(zmqpp::context &ctx,
                                 zmqpp::socket *pipe,
                                 boost::property_tree::ptree const &cfg,
                                 Scheduler &sched) :
        BaseModule(ctx, pipe, cfg, sched)
{
    process_config();
    for (auto &led : leds_)
    {
        reactor_.add(led->frontend(), std::bind(&LedBuzzerImpl::handle_message, led.get()));
    }
}

int LEDBuzzerModule::compute_timeout()
{
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::time_point::max();
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

    for (auto &led : leds_)
    {
        if (led->next_update() < tp)
        {
            tp = led->next_update();
        }
    }
    if (tp == std::chrono::system_clock::time_point::max())
        return -1; // no update asked.

    int timeout = std::chrono::duration_cast<std::chrono::milliseconds>(tp - now).count();
    return timeout < 0 ? 0 : timeout;
}

void LEDBuzzerModule::run()
{
    while (is_running_)
    {
        reactor_.poll(compute_timeout());
        for (auto &led : leds_)
        {
            if (led->next_update() <= std::chrono::system_clock::now())
                led->update();
        }
    }
}

void LEDBuzzerModule::process_config()
{
    boost::property_tree::ptree module_config = config_.get_child("module_config");

    if (const auto &leds_node = module_config.get_child_optional("leds"))
    {
        for (auto &node : *leds_node)
        {
            boost::property_tree::ptree led_cfg = node.second;

            std::string led_name = led_cfg.get_child("name").data();
            std::string gpio_name = led_cfg.get_child("gpio").data();
            int default_blink_duration = led_cfg.get<int>("default_blink_duration", 1000);
            int default_blink_speed = led_cfg.get<int>("default_blink_speed", 200);

            INFO("Creating LED " << led_name << ", linked to GPIO " << gpio_name);
            leds_.push_back(std::shared_ptr<LedBuzzerImpl>(new LedBuzzerImpl(ctx_, led_name, gpio_name, default_blink_duration, default_blink_speed)));
        }
    }

    if (const auto &buzzers_node = module_config.get_child_optional("buzzers"))
    {
        for (auto &node : *buzzers_node)
        {
            boost::property_tree::ptree buzzer_cfg = node.second;

            std::string buzzer_name = buzzer_cfg.get_child("name").data();
            std::string gpio_name = buzzer_cfg.get_child("gpio").data();
            int default_blink_duration = buzzer_cfg.get<int>("default_blink_duration", 1000);
            int default_blink_speed = buzzer_cfg.get<int>("default_blink_speed", 200);

            INFO("Creating Buzzer " << buzzer_name << ", linked to GPIO " << gpio_name);
            // internally we do not care if its a buzzer or a led.
            leds_.push_back(std::shared_ptr<LedBuzzerImpl>(new LedBuzzerImpl(ctx_, buzzer_name, gpio_name, default_blink_duration, default_blink_speed)));
        }
    }
}
