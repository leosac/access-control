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
#include <boost/property_tree/ptree.hpp>
#include <tools/log.hpp>
#include <zmqpp/context.hpp>
#include <fcntl.h>
#include "wiegand.hpp"
#include "zmqpp/actor.hpp"

using namespace Leosac::Module::Wiegand;

extern "C"
{
const char *get_module_name()
{
    return "WIEGAND_READER";
}
}

/**
* Entry point of wiegand module.
* This module provide support for controlling WiegandReader.
*/
extern "C" __attribute__((visibility("default"))) bool start_module(zmqpp::socket *pipe,
        boost::property_tree::ptree cfg,
        zmqpp::context &zmq_ctx)
{
    return Leosac::Module::start_module_helper<WiegandReaderModule>(pipe, cfg, zmq_ctx);
}

WiegandReaderModule::WiegandReaderModule(zmqpp::context &ctx,
        zmqpp::socket *pipe,
        boost::property_tree::ptree const &cfg) :
        BaseModule(ctx, pipe, cfg)
{
    process_config();

    for (auto &reader : readers_)
    {
        reactor_.add(reader.bus_sub_, std::bind(&WiegandReaderImpl::handle_bus_msg, &reader));
        reactor_.add(reader.sock_, std::bind(&WiegandReaderImpl::handle_request, &reader));
    }
}

void WiegandReaderModule::process_config()
{
    boost::property_tree::ptree module_config = config_.get_child("module_config");

    for (auto &node : module_config.get_child("readers"))
    {
        boost::property_tree::ptree reader_cfg = node.second;

        std::string reader_name = reader_cfg.get_child("name").data();
        std::string gpio_high = reader_cfg.get_child("high").data();
        std::string gpio_low = reader_cfg.get_child("low").data();
        std::string buzzer_name = reader_cfg.get<std::string>("buzzer", "");
        std::string greenled_name = reader_cfg.get<std::string>("green_led", "");

        INFO("Creating READER " << reader_name << " (green led = " << greenled_name << "), buzzer {" << buzzer_name << "}");
        WiegandReaderImpl reader(ctx_, reader_name, gpio_high, gpio_low, greenled_name, buzzer_name);
        readers_.push_back(std::move(reader));
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
