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

#include "SysFsGpioModule.hpp"
#include "SysFsGpioConfig.hpp"
#include "core/kernel.hpp"
#include "tools/log.hpp"
#include "tools/timeout.hpp"
#include "tools/unixfs.hpp"
#include <boost/iterator/transform_iterator.hpp>
#include <boost/property_tree/ptree.hpp>
#include <memory>
#include <zmqpp/context.hpp>
#include <zmqpp/message.hpp>


using namespace Leosac::Module::SysFsGpio;
using Leosac::Tools::UnixFs;


SysFsGpioModule::SysFsGpioModule(zmqpp::context &ctx,
                                 zmqpp::socket *module_manager_pipe,
                                 const boost::property_tree::ptree &config,
                                 CoreUtilsPtr utils)
    : BaseModule(ctx, module_manager_pipe, config, utils)
    , bus_push_(ctx_, zmqpp::socket_type::push)
    , general_cfg_(nullptr)
{
    bus_push_.connect("inproc://zmq-bus-pull");
    process_config(config);

    for (auto &gpio : gpios_)
    {
        gpio->register_sockets(&reactor_);
    }
}

static SysFsGpioPin::InterruptMode gpio_interrupt_from_string(const std::string &str)
{
    if (str == "none")
        return SysFsGpioPin::InterruptMode::None;
    else if (str == "both")
        return SysFsGpioPin::InterruptMode::Both;
    else if (str == "falling")
        return SysFsGpioPin::InterruptMode::Falling;
    else if (str == "rising")
        return SysFsGpioPin::InterruptMode::Rising;
    else
        return SysFsGpioPin::InterruptMode::None;
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

        gpio_name          = gpio_cfg.get_child("name").data();
        gpio_no            = std::stoi(gpio_cfg.get_child("no").data());
        gpio_direction     = gpio_cfg.get_child("direction").data();
        gpio_interrupt     = gpio_cfg.get<std::string>("interrupt_mode", "none");
        gpio_initial_value = gpio_cfg.get<bool>("value", false);

        using namespace Colorize;
        INFO("Creating GPIO " << green(underline(gpio_name)) << ", with no "
                              << green(underline(gpio_no)) << ". direction = "
                              << green(underline(gpio_direction)));

        export_gpio(gpio_no);
        interrupt_mode = gpio_interrupt_from_string(gpio_interrupt);

        direction = (gpio_direction == "in" ? SysFsGpioPin::Direction::In
                                            : SysFsGpioPin::Direction::Out);
        gpios_.push_back(new SysFsGpioPin(ctx_, gpio_name, gpio_no, direction,
                                          interrupt_mode, gpio_initial_value,
                                          *this));

        utils_->config_checker().register_object(gpio_name,
                                                 ConfigChecker::ObjectType::GPIO);
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

void SysFsGpioModule::process_general_config()
{
    assert(general_cfg_ == nullptr);
    general_cfg_ = new SysFsGpioConfig(config_.get_child("module_config"));
}

const SysFsGpioConfig &SysFsGpioModule::general_config() const
{
    return *general_cfg_;
}

void SysFsGpioModule::run()
{
    while (is_running_)
    {
        auto itr_transform =
            [](const SysFsGpioPin *p) -> std::chrono::system_clock::time_point {
            return p->next_update();
        };

        auto timeout = Tools::compute_timeout(
            boost::make_transform_iterator(gpios_.begin(), itr_transform),
            boost::make_transform_iterator(gpios_.end(), itr_transform));
        reactor_.poll(timeout);
        for (auto &gpio_pin : gpios_)
        {
            if (gpio_pin->next_update() < std::chrono::system_clock::now())
                gpio_pin->update();
        }
    }
}
