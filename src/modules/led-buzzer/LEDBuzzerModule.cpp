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

#include "LEDBuzzerModule.hpp"
#include "core/kernel.hpp"
#include "hardware/Buzzer_odb.h"
#include "hardware/GPIO.hpp"
#include "hardware/LED_odb.h"
#include "tools/db/database.hpp"
#include "tools/timeout.hpp"
#include "ws/WSHelperThread.hpp"
#include <boost/iterator/transform_iterator.hpp>

namespace Leosac
{
namespace Module
{
namespace LedBuzzer
{

LEDBuzzerModule::LEDBuzzerModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                                 boost::property_tree::ptree const &cfg,
                                 CoreUtilsPtr utils)
    : BaseModule(ctx, pipe, cfg, utils)
{
    process_config();
    for (auto &led : leds_and_buzzers_)
    {
        reactor_.add(led->frontend(),
                     std::bind(&LedBuzzerImpl::handle_message, led.get()));
    }
}

LEDBuzzerModule::~LEDBuzzerModule()
{
}

void LEDBuzzerModule::run()
{
    if (config_.get_child("module_config").get<bool>("use_database", false))
    {
        ws_helper_thread_ = std::make_unique<WSHelperThread>(utils_);
        ws_helper_thread_->start_running();
    }
    while (is_running_)
    {
        auto itr_transform = [](const std::shared_ptr<LedBuzzerImpl> &lb) {
            return lb->next_update();
        };
        reactor_.poll(Tools::compute_timeout(
            boost::make_transform_iterator(leds_and_buzzers_.begin(), itr_transform),
            boost::make_transform_iterator(leds_and_buzzers_.end(), itr_transform)));
        for (auto &led : leds_and_buzzers_)
        {
            if (led->next_update() <= std::chrono::system_clock::now())
                led->update();
        }
    }
    auto ws_service = get_service_registry().get_service<WebSockAPI::Service>();
    if (ws_service && ws_helper_thread_)
        ws_helper_thread_->unregister_ws_handlers(*ws_service);
}

void LEDBuzzerModule::process_config()
{
    boost::property_tree::ptree module_config = config_.get_child("module_config");

    if (module_config.get<bool>("use_database", false))
    {
        load_db_config();
    }
    else
    {
        load_xml_config();
    }
}

void LEDBuzzerModule::load_xml_config()
{
    boost::property_tree::ptree module_config = config_.get_child("module_config");

    if (const auto &leds_node = module_config.get_child_optional("leds"))
    {
        for (auto &node : *leds_node)
        {
            boost::property_tree::ptree led_cfg = node.second;

            std::string led_name  = led_cfg.get<std::string>("name");
            std::string gpio_name = led_cfg.get<std::string>("gpio");
            int default_blink_duration =
                led_cfg.get<int>("default_blink_duration", 1000);
            int default_blink_speed = led_cfg.get<int>("default_blink_speed", 200);

            INFO("Creating LED " << led_name << ", linked to GPIO " << gpio_name);
            config_check(gpio_name, Leosac::Hardware::DeviceClass::GPIO);

            leds_and_buzzers_.push_back(std::make_shared<LedBuzzerImpl>(
                ctx_, led_name, gpio_name, default_blink_duration,
                default_blink_speed));
            utils_->config_checker().register_object(led_name,
                                                     Leosac::Hardware::DeviceClass::LED);
        }
    }

    if (const auto &buzzers_node = module_config.get_child_optional("buzzers"))
    {
        for (auto &node : *buzzers_node)
        {
            boost::property_tree::ptree buzzer_cfg = node.second;

            std::string buzzer_name = buzzer_cfg.get<std::string>("name");
            std::string gpio_name   = buzzer_cfg.get<std::string>("gpio");
            int default_blink_duration =
                buzzer_cfg.get<int>("default_blink_duration", 1000);
            int default_blink_speed =
                buzzer_cfg.get<int>("default_blink_speed", 200);

            INFO("Creating Buzzer " << buzzer_name << ", linked to GPIO "
                                    << gpio_name);
            config_check(gpio_name, Leosac::Hardware::DeviceClass::GPIO);

            // internally we do not care if its a buzzer or a led.
            leds_and_buzzers_.push_back(std::make_shared<LedBuzzerImpl>(
                ctx_, buzzer_name, gpio_name, default_blink_duration,
                default_blink_speed));
            utils_->config_checker().register_object(
                buzzer_name, Leosac::Hardware::DeviceClass::BUZZER);
        }
    }
}

void LEDBuzzerModule::load_db_config()
{
    using namespace odb;
    using namespace odb::core;
    auto db = utils_->database();

    // Load leds
    {
        odb::transaction t(utils_->database()->begin());
        odb::result<Hardware::LED> result(
            utils_->database()->query<Hardware::LED>());
        for (const auto &led : result)
        {
            if (!led.enabled())
                continue;
            if (!led.gpio())
            {
                WARN("No GPIO associated with device " << led.name()
                                                       << ". Skipping.");
                continue;
            }
            leds_and_buzzers_.push_back(std::make_shared<LedBuzzerImpl>(
                ctx_, led.name(), led.gpio()->name(), led.default_blink_duration(),
                led.default_blink_speed()));
            utils_->config_checker().register_object(led.name(),
                                                     Leosac::Hardware::DeviceClass::LED);
        }
        t.commit();
    }

    // Load buzzer
    {
        odb::transaction t(utils_->database()->begin());
        odb::result<Hardware::Buzzer> result(
            utils_->database()->query<Hardware::Buzzer>());
        for (const auto &buzzer : result)
        {
            if (!buzzer.enabled())
                continue;
            if (!buzzer.gpio())
            {
                WARN("No GPIO associated with device " << buzzer.name()
                                                       << ". Skipping.");
                continue;
            }
            leds_and_buzzers_.push_back(std::make_shared<LedBuzzerImpl>(
                ctx_, buzzer.name(), buzzer.gpio()->name(),
                buzzer.default_blink_duration(), buzzer.default_blink_speed()));
            utils_->config_checker().register_object(
                buzzer.name(), Leosac::Hardware::DeviceClass::BUZZER);
        }
        t.commit();
    }

    INFO("LEDBuzzer module using SQL database for configuration.");
}
}
}
}
