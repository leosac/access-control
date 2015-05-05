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
#include "SysFsGpioModule.hpp"
#include "SysFsGpioConfig.hpp"

using namespace Leosac::Module::SysFsGpio;
using Leosac::Tools::UnixFs;


SysFsGpioModule::SysFsGpioModule(zmqpp::context &ctx,
                                 zmqpp::socket *module_manager_pipe,
                                 const boost::property_tree::ptree &config,
                                 Scheduler &sched)
        : BaseModule(ctx, module_manager_pipe, config, sched),
          bus_push_(ctx_, zmqpp::socket_type::push),
          general_cfg_(nullptr)
{
    bus_push_.connect("inproc://zmq-bus-pull");
    process_config(config);

    for (auto &gpio : gpios_)
    {
        gpio->register_sockets(&reactor_);
    }
}

void SysFsGpioModule::process_config(const boost::property_tree::ptree &cfg)
{
    boost::property_tree::ptree module_config = cfg.get_child("module_config");

    process_general_config();

    for (auto &node : module_config.get_child("gpios"))
    {
        boost::property_tree::ptree gpio_cfg = node.second;
        SysFsGpioPin::Direction direction;
        SysFsGpioPin::InterruptMode interrupt_mode;
        std::string gpio_name;
        std::string gpio_direction;
        std::string gpio_interrupt;
        int gpio_no;
        bool gpio_initial_value;

        gpio_name = gpio_cfg.get_child("name").data();
        gpio_no = std::stoi(gpio_cfg.get_child("no").data());
        gpio_direction = gpio_cfg.get_child("direction").data();
        gpio_interrupt = gpio_cfg.get<std::string>("interrupt_mode", "none");
        gpio_initial_value = gpio_cfg.get<bool>("value", false);

        INFO("Creating GPIO " << gpio_name << ", with no " << gpio_no <<
             ". direction = " << gpio_direction);

        export_gpio(gpio_no);

        if (gpio_interrupt == "none")
            interrupt_mode = SysFsGpioPin::InterruptMode::None;
        else if (gpio_interrupt == "both")
            interrupt_mode = SysFsGpioPin::InterruptMode::Both;
        else if (gpio_interrupt == "falling")
            interrupt_mode = SysFsGpioPin::InterruptMode::Falling;
        else if (gpio_interrupt == "rising")
            interrupt_mode = SysFsGpioPin::InterruptMode::Rising;

        direction = (gpio_direction == "in" ? SysFsGpioPin::Direction::In
                                            : SysFsGpioPin::Direction::Out);
        gpios_.push_back(
                new SysFsGpioPin(ctx_, gpio_name, gpio_no, direction, interrupt_mode,
                                 gpio_initial_value, *this));
    }
}

void SysFsGpioModule::export_gpio(int gpio_no)
{
    UnixFs::writeSysFsValue(general_cfg_->export_path(), gpio_no);
}

SysFsGpioModule::~SysFsGpioModule()
{
    for (auto gpio : gpios_)
        delete gpio;
    delete general_cfg_;
}

void SysFsGpioModule::publish_on_bus(zmqpp::message &msg)
{
    bus_push_.send(msg);
}

bool SysFsGpioModule::process_general_config()
{
    try
    {
        assert(general_cfg_ == nullptr);
        general_cfg_ = new SysFsGpioConfig(config_.get_child("module_config"));
        return true;
    }
    catch (std::exception &e)
    {
        ERROR("SysFsGpio Invalid Configuration:" << e.what());
        throw e;
    }
}

const SysFsGpioConfig &SysFsGpioModule::general_config() const
{
    return *general_cfg_;
}
