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

#include <memory>
#include <zmqpp/message.hpp>
#include <zmqpp/context.hpp>
#include <boost/property_tree/ptree.hpp>
#include <modules/wiegand/strategies/Autodetect.hpp>
#include "tools/log.hpp"
#include "wiegand.hpp"
#include "core/Scheduler.hpp"
#include "core/kernel.hpp"

using namespace Leosac::Module::Wiegand;

extern "C" {
const char *get_module_name()
{
    return "WIEGAND_READER";
}
}

/**
* Entry point of wiegand module.
* This module provide support for controlling WiegandReader.
*/
extern "C" __attribute__((visibility("default"))) bool
start_module(zmqpp::socket *pipe, boost::property_tree::ptree cfg,
             zmqpp::context &zmq_ctx, Leosac::CoreUtilsPtr utils)
{
    return Leosac::Module::start_module_helper<WiegandReaderModule>(pipe, cfg,
                                                                    zmq_ctx, utils);
}

WiegandReaderModule::WiegandReaderModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                                         boost::property_tree::ptree const &cfg,
                                         CoreUtilsPtr utils)
    : BaseModule(ctx, pipe, cfg, utils)
{
    process_config();

    for (auto &reader : readers_)
    {
        reactor_.add(reader.bus_sub_,
                     std::bind(&WiegandReaderImpl::handle_bus_msg, &reader));
        reactor_.add(reader.sock_,
                     std::bind(&WiegandReaderImpl::handle_request, &reader));
    }
}

void WiegandReaderModule::process_config()
{
    boost::property_tree::ptree module_config = config_.get_child("module_config");

    for (auto &node : module_config.get_child("readers"))
    {
        boost::property_tree::ptree reader_cfg = node.second;

        std::string reader_name   = reader_cfg.get_child("name").data();
        std::string gpio_high     = reader_cfg.get_child("high").data();
        std::string gpio_low      = reader_cfg.get_child("low").data();
        std::string buzzer_name   = reader_cfg.get<std::string>("buzzer", "");
        std::string greenled_name = reader_cfg.get<std::string>("green_led", "");

        using namespace Colorize;
        INFO("Creating WiegandReader: "
             << green(underline(reader_name))
             << "\n\t Green Led: " << green(underline(greenled_name))
             << "\n\t Buzzer: " << green(underline(buzzer_name))
             << "\n\t GPIO Low: " << green(underline(gpio_low))
             << "\n\t GPIO High: " << green(underline(gpio_high)));

        config_check(gpio_low, ConfigChecker::ObjectType::GPIO);
        config_check(gpio_high, ConfigChecker::ObjectType::GPIO);

        if (greenled_name.size())
            config_check(greenled_name, ConfigChecker::ObjectType::LED);
        if (buzzer_name.size())
            config_check(buzzer_name, ConfigChecker::ObjectType::BUZZER);

        WiegandReaderImpl reader(ctx_, reader_name, gpio_high, gpio_low,
                                 greenled_name, buzzer_name,
                                 create_strategy(reader_cfg, &reader));
        readers_.push_back(std::move(reader));
        utils_->config_checker().register_object(reader_name,
                                                 ConfigChecker::ObjectType::READER);
    }
}

void WiegandReaderModule::run()
{
    while (is_running_)
    {
        if (!reactor_.poll(50))
        {
            for (auto &reader : readers_)
                reader.timeout();
        }
    }
}

Strategy::WiegandStrategyUPtr
WiegandReaderModule::create_strategy(const boost::property_tree::ptree &reader_cfg,
                                     WiegandReaderImpl *reader)
{
    using namespace Auth;
    using namespace Strategy;
    std::string mode = reader_cfg.get<std::string>("mode", "SIMPLE_WIEGAND");
    std::chrono::milliseconds pin_timeout =
        std::chrono::milliseconds(reader_cfg.get<int>("pin_timeout", 2500));
    char pin_key_end = reader_cfg.get<char>("pin_key_end", '#');

    auto simple_wiegand =
        std::unique_ptr<CardReading>(new SimpleWiegandStrategy(reader));
    auto pin_4bits = std::unique_ptr<PinReading>(
        new WiegandPinNBitsOnly<4>(reader, pin_timeout, pin_key_end));
    auto pin_8bits = std::unique_ptr<PinReading>(
        new WiegandPinNBitsOnly<8>(reader, pin_timeout, pin_key_end));
    auto pin_buffered = std::unique_ptr<PinReading>(new WiegandPinBuffered(reader));

    if (mode == "SIMPLE_WIEGAND")
        return std::move(simple_wiegand);
    else if (mode == "WIEGAND_PIN_4BITS")
        return std::move(pin_4bits);
    else if (mode == "WIEGAND_PIN_8BITS")
        return std::move(pin_8bits);
    else if (mode == "WIEGAND_PIN_BUFFERED")
        return std::move(pin_buffered);
    else if (mode == "WIEGAND_CARD_PIN_4BITS")
    {
        return std::unique_ptr<WiegandStrategy>(new WiegandCardAndPin(
            reader, std::move(simple_wiegand), std::move(pin_4bits), pin_timeout));
    }
    else if (mode == "WIEGAND_CARD_PIN_8BITS")
    {
        return std::unique_ptr<WiegandStrategy>(new WiegandCardAndPin(
            reader, std::move(simple_wiegand), std::move(pin_8bits), pin_timeout));
    }
    else if (mode == "WIEGAND_CARD_PIN_BUFFERED")
    {
        return std::unique_ptr<WiegandStrategy>(
            new WiegandCardAndPin(reader, std::move(simple_wiegand),
                                  std::move(pin_buffered), pin_timeout));
    }
    else if (mode == "AUTODETECT")
    {
        return std::unique_ptr<WiegandStrategy>(
            new Autodetect(reader, pin_timeout, pin_key_end));
    }
    else
    {
        ERROR("Wiegand mode " << mode << " is not a valid mode.");
        assert(0);
        throw std::runtime_error("Invalid wiegand mode " + mode);
    }
    return nullptr;
}
