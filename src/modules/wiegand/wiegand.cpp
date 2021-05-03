/*
    Copyright (C) 2014-2016 Leosac

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

#include "modules/wiegand/wiegand.hpp"
#include "core/Scheduler.hpp"
#include "core/kernel.hpp"
#include "hardware/Buzzer.hpp"
#include "hardware/HardwareService.hpp"
#include "hardware/LED.hpp"
#include "modules/wiegand/WiegandConfig_odb.h"
#include "modules/wiegand/strategies/Autodetect.hpp"
#include "modules/wiegand/ws/WSHelperThread.hpp"
#include "tools/log.hpp"
#include "ws/WiegandConfigSerializer.hpp"
#include <boost/property_tree/ptree.hpp>
#include <memory>
#include <zmqpp/context.hpp>
#include <zmqpp/message.hpp>

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

WiegandReaderModule::~WiegandReaderModule()
{
    auto hwd_service =
        get_service_registry().get_service<Hardware::HardwareService>();
    if (hwd_service)
        hwd_service->unregister_serializer<WiegandReaderConfig>();
}

void WiegandReaderModule::process_config()
{
    boost::property_tree::ptree module_config = config_.get_child("module_config");

    if (module_config.get<bool>("use_database", false))
    {
        auto hwd_service =
            get_service_registry().get_service<Hardware::HardwareService>();
        ASSERT_LOG(hwd_service, "No hardware service but we have database.");
        hwd_service->register_serializer<WiegandReaderConfig>(
            &WiegandReaderConfigSerializer::serialize);
        load_db_config();
    }
    else
    {
        load_xml_config(module_config);
    }

    // Now we process the configuration object.
    for (const auto &reader_config : wiegand_config_->readers())
    {
        using namespace Colorize;
        INFO("Creating WiegandReader: "
             << green(underline(reader_config->name())) << "\n\t Green Led: "
             << green(underline(reader_config->green_led_name()))
             << "\n\t Buzzer: " << green(underline(reader_config->buzzer_name()))
             << "\n\t GPIO Low: " << green(underline(reader_config->gpio_low_name()))
             << "\n\t GPIO High: "
             << green(underline(reader_config->gpio_high_name()))
             << "\n\t Mode: " << green(underline(reader_config->mode)));

        WiegandReaderImpl reader(
            ctx_, reader_config->name(), reader_config->gpio_high_name(),
            reader_config->gpio_low_name(), reader_config->green_led_name(),
            reader_config->buzzer_name(), create_strategy(*reader_config, &reader));
        utils_->config_checker().register_object(reader.name(),
                                                 ConfigChecker::ObjectType::READER);
        readers_.push_back(std::move(reader));
    }
}

void WiegandReaderModule::run()
{
    if (config_.get_child("module_config").get<bool>("use_database", false))
    {
        ws_helper_thread_ = std::make_unique<WSHelperThread>(utils_);
        ws_helper_thread_->start_running();
    }
    while (is_running_)
    {
        if (!reactor_.poll(50))
        {
            for (auto &reader : readers_)
                reader.timeout();
        }
    }
    auto ws_service = get_service_registry().get_service<WebSockAPI::Service>();
    if (ws_service && ws_helper_thread_)
        ws_helper_thread_->unregister_ws_handlers(*ws_service);
}

Strategy::WiegandStrategyUPtr
WiegandReaderModule::create_strategy(const WiegandReaderConfig &reader_cfg,
                                     WiegandReaderImpl *reader)
{
    using namespace Auth;
    using namespace Strategy;

    auto simple_wiegand =
        std::unique_ptr<CardReading>(new SimpleWiegandStrategy(reader));
    auto pin_4bits = std::unique_ptr<PinReading>(new WiegandPinNBitsOnly<4>(
        reader, reader_cfg.pin_timeout, reader_cfg.pin_key_end));
    auto pin_8bits = std::unique_ptr<PinReading>(new WiegandPinNBitsOnly<8>(
        reader, reader_cfg.pin_timeout, reader_cfg.pin_key_end));
    auto pin_buffered = std::unique_ptr<PinReading>(new WiegandPinBuffered(reader));

    if (reader_cfg.mode == "SIMPLE_WIEGAND")
        return std::move(simple_wiegand);
    else if (reader_cfg.mode == "WIEGAND_PIN_4BITS")
        return std::move(pin_4bits);
    else if (reader_cfg.mode == "WIEGAND_PIN_8BITS")
        return std::move(pin_8bits);
    else if (reader_cfg.mode == "WIEGAND_PIN_BUFFERED")
        return std::move(pin_buffered);
    else if (reader_cfg.mode == "WIEGAND_CARD_PIN_4BITS")
    {
        return std::unique_ptr<WiegandStrategy>(
            new WiegandCardAndPin(reader, std::move(simple_wiegand),
                                  std::move(pin_4bits), reader_cfg.pin_timeout));
    }
    else if (reader_cfg.mode == "WIEGAND_CARD_PIN_8BITS")
    {
        return std::unique_ptr<WiegandStrategy>(
            new WiegandCardAndPin(reader, std::move(simple_wiegand),
                                  std::move(pin_8bits), reader_cfg.pin_timeout));
    }
    else if (reader_cfg.mode == "WIEGAND_CARD_PIN_BUFFERED")
    {
        return std::unique_ptr<WiegandStrategy>(
            new WiegandCardAndPin(reader, std::move(simple_wiegand),
                                  std::move(pin_buffered), reader_cfg.pin_timeout));
    }
    else if (reader_cfg.mode == "AUTODETECT")
    {
        return std::unique_ptr<WiegandStrategy>(
            new Autodetect(reader, reader_cfg.pin_timeout, reader_cfg.pin_key_end, reader_cfg.nowait));
    }
    else
    {
        ERROR("Wiegand mode " << reader_cfg.mode << " is not a valid mode.");
        assert(0);
        throw std::runtime_error("Invalid wiegand mode " + reader_cfg.mode);
    }
    return nullptr;
}

void WiegandReaderModule::load_db_config()
{
    using namespace odb;
    using namespace odb::core;
    auto db = utils_->database();

    // First we load or update database schema if needed.
    schema_version v = db->schema_version("module_wiegand");
    schema_version cv(schema_catalog::current_version(*db, "module_wiegand"));
    if (v == 0)
    {
        transaction t(db->begin());
        INFO("Attempt to create module_wiegand SQL schema.");
        schema_catalog::create_schema(*db, "module_wiegand");
        t.commit();
    }
    else if (v < cv)
    {
        INFO("Wiegand Module performing database migration. Going from version "
             << v << " to version " << cv);
        transaction t(db->begin());
        schema_catalog::migrate(*db, cv, "module_wiegand");
        t.commit();
    }

    // Create empty configuration object...
    wiegand_config_ = std::make_unique<WiegandConfig>();

    // ... then loads reader from database.
    odb::transaction t(utils_->database()->begin());
    odb::result<WiegandReaderConfig> result(
        utils_->database()->query<WiegandReaderConfig>());
    for (const auto &reader : result)
    {
        WiegandReaderConfigPtr reader_ptr =
            db->load<WiegandReaderConfig>(reader.id());
        ASSERT_LOG(reader_ptr, "Loading from database/cache failed");
        if (reader_ptr->enabled())
            wiegand_config_->add_reader(reader_ptr);
    }
    t.commit();
    INFO("Wiegand module using SQL database for configuration.");
}

void WiegandReaderModule::load_xml_config(
    const boost::property_tree::ptree &module_config)
{
    wiegand_config_ = std::make_unique<WiegandConfig>();
    for (auto &node : module_config.get_child("readers"))
    {
        auto reader_config = std::make_shared<WiegandReaderConfig>();
        boost::property_tree::ptree xml_reader_cfg = node.second;

        // For GPIO object, we instanciate a "dummy" GPIO object that would
        // normally be fetched from the database. The only goal of the GPIO
        // object is to hold the device's name

        auto gpio_high = std::make_shared<Hardware::GPIO>();
        gpio_high->name(xml_reader_cfg.get<std::string>("high"));

        auto gpio_low = std::make_shared<Hardware::GPIO>();
        gpio_low->name(xml_reader_cfg.get<std::string>("low"));

        auto green_led = std::make_shared<Hardware::LED>();
        green_led->name(xml_reader_cfg.get<std::string>("green_led", ""));

        auto buzzer = std::make_shared<Hardware::Buzzer>();
        buzzer->name(xml_reader_cfg.get<std::string>("buzzer", ""));

        reader_config->name(xml_reader_cfg.get_child("name").data());
        reader_config->gpio_high_ = gpio_high;
        reader_config->gpio_low_  = gpio_low;
        reader_config->buzzer_    = buzzer;
        reader_config->green_led_ = green_led;

        reader_config->mode =
            xml_reader_cfg.get<std::string>("mode", "SIMPLE_WIEGAND");
        reader_config->pin_timeout =
            std::chrono::milliseconds(xml_reader_cfg.get<int>("pin_timeout", 2500));
        reader_config->pin_key_end = xml_reader_cfg.get<char>("pin_key_end", '#');
        reader_config->nowait = xml_reader_cfg.get<bool>("nowait", 0);

        config_check(reader_config->gpio_low_name(),
                     ConfigChecker::ObjectType::GPIO);
        config_check(reader_config->gpio_high_name(),
                     ConfigChecker::ObjectType::GPIO);

        if (!reader_config->green_led_name().empty())
            config_check(reader_config->green_led_name(),
                         ConfigChecker::ObjectType::LED);
        if (!reader_config->buzzer_name().empty())
            config_check(reader_config->buzzer_name(),
                         ConfigChecker::ObjectType::BUZZER);

        wiegand_config_->add_reader(reader_config);
    }
}
