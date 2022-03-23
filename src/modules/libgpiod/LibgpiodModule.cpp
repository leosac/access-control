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

#include "LibgpiodModule.hpp"
#include "LibgpiodConfig.hpp"
#include "core/kernel.hpp"
#include "tools/log.hpp"
#include "tools/timeout.hpp"
#include <boost/iterator/transform_iterator.hpp>
#include <boost/property_tree/ptree.hpp>
#include <memory>
#include <zmqpp/context.hpp>
#include <zmqpp/message.hpp>


using namespace Leosac::Module::Libgpiod;


LibgpiodModule::LibgpiodModule(zmqpp::context &ctx,
                                 zmqpp::socket *module_manager_pipe,
                                 const boost::property_tree::ptree &config,
                                 CoreUtilsPtr utils)
    : BaseModule(ctx, module_manager_pipe, config, utils)
    , bus_push_(ctx_, zmqpp::socket_type::push)
    , general_cfg_(nullptr)
{
    bus_push_.connect("inproc://zmq-bus-pull");
    process_config(config);

    for (auto gpio : gpios_)
    {
        gpio->register_sockets(&reactor_);
    }
}

static LibgpiodPin::InterruptMode gpio_interrupt_from_string(const std::string &str)
{
    if (str == "none")
        return LibgpiodPin::InterruptMode::None;
    else if (str == "both")
        return LibgpiodPin::InterruptMode::Both;
    else if (str == "falling")
        return LibgpiodPin::InterruptMode::Falling;
    else if (str == "rising")
        return LibgpiodPin::InterruptMode::Rising;
    else
        return LibgpiodPin::InterruptMode::None;
}

void LibgpiodModule::process_config(const boost::property_tree::ptree &cfg)
{
    boost::property_tree::ptree module_config = cfg.get_child("module_config");

    process_general_config();

    for (auto &node : module_config.get_child("gpios"))
    {
        boost::property_tree::ptree gpio_cfg = node.second;
        LibgpiodPin::Direction direction;
        LibgpiodPin::InterruptMode interrupt_mode;
        std::string gpio_name;
        std::string gpio_device;
        std::string gpio_direction;
        std::string gpio_interrupt;
        int gpio_offset;
        bool gpio_initial_value;

        gpio_name          = gpio_cfg.get_child("name").data();
        gpio_device        = gpio_cfg.get<std::string>("device", "leosac");
        gpio_offset        = std::stoi(gpio_cfg.get_child("offset").data());
        gpio_direction     = gpio_cfg.get_child("direction").data();
        gpio_interrupt     = gpio_cfg.get<std::string>("interrupt_mode", "none");
        gpio_initial_value = gpio_cfg.get<bool>("value", false);

        using namespace Colorize;
        INFO("Creating GPIO " << green(underline(gpio_name)) << ", device "
                              << green(underline(gpio_device)) << " with offset "
                              << green(underline(gpio_offset)) << ". direction = "
                              << green(underline(gpio_direction)));

        interrupt_mode = gpio_interrupt_from_string(gpio_interrupt);

        direction = (gpio_direction == "in" ? LibgpiodPin::Direction::In
                                            : LibgpiodPin::Direction::Out);
        gpios_.push_back(std::make_shared<LibgpiodPin>(ctx_, gpio_name, gpio_device, gpio_offset, direction,
                                          interrupt_mode, gpio_initial_value,
                                          *this));

        utils_->config_checker().register_object(gpio_name,
                                                 Leosac::Hardware::DeviceClass::GPIO);
    }
}

LibgpiodModule::~LibgpiodModule()
{
    for (auto gpio : gpios_)
        gpio->release();
}

void LibgpiodModule::publish_on_bus(zmqpp::message &msg)
{
    bus_push_.send(msg);
}

void LibgpiodModule::process_general_config()
{
    assert(general_cfg_ == nullptr);
    general_cfg_ = std::make_shared<LibgpiodConfig>(config_.get_child("module_config"));
}

std::shared_ptr<LibgpiodConfig> LibgpiodModule::general_config() const
{
    return general_cfg_;
}

void LibgpiodModule::run()
{
    while (is_running_)
    {
        auto itr_transform =
            [](std::shared_ptr<const LibgpiodPin> p) -> std::chrono::system_clock::time_point {
            return p->next_update();
        };

        auto timeout = Tools::compute_timeout(
            boost::make_transform_iterator(gpios_.begin(), itr_transform),
            boost::make_transform_iterator(gpios_.end(), itr_transform));
        reactor_.poll(timeout);
        for (auto gpio_pin : gpios_)
        {
            if (gpio_pin->next_update() < std::chrono::system_clock::now())
                gpio_pin->update();
        }
    }
}
