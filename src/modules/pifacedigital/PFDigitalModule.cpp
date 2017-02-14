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

#include "PFDigitalModule.hpp"
#include "core/CoreUtils.hpp"
#include "exception/gpioexception.hpp"
#include "mcp23s17.h"
#include "pifacedigital.h"
#include "tools/enforce.hpp"
#include "tools/log.hpp"
#include "tools/timeout.hpp"
#include <boost/iterator/transform_iterator.hpp>
#include <fcntl.h>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::Piface;

PFDigitalModule::PFDigitalModule(zmqpp::context &ctx,
                                 zmqpp::socket *module_manager_pipe,
                                 const boost::property_tree::ptree &config,
                                 CoreUtilsPtr utils)
    : BaseModule(ctx, module_manager_pipe, config, utils)
    , bus_push_(ctx_, zmqpp::socket_type::push)
{
    if (pifacedigital_open(0) == -1)
    {
        ERROR("Cannot open PifaceDigital device");
        throw LEOSACException(
            "Cannot open PifaceDigital device (is SPI module enabled ?)");
    }
    int ret = pifacedigital_enable_interrupts();
    ASSERT_LOG(ret == 0, "Failed to enable interrupt on piface board");

    process_config(config);
    bus_push_.connect("inproc://zmq-bus-pull");
    for (auto &gpio : gpios_)
    {
        reactor_.add(gpio.sock_, std::bind(&PFDigitalPin::handle_message, &gpio));
    }

    std::string path_to_gpio =
        "/sys/class/gpio/gpio" + std::to_string(GPIO_INTERRUPT_PIN) + "/value";
    interrupt_fd_ = open(path_to_gpio.c_str(), O_RDONLY | O_NONBLOCK);
    LEOSAC_ENFORCE(interrupt_fd_ > 0, "Failed to open GPIO file");
    pifacedigital_read_reg(0x11, 0); // flush

    // Somehow it was required poll with "poll_pri" and "poll_errror". It used to
    // work with poll_pri alone before. Need to investigate more. todo !
    reactor_.add(interrupt_fd_, std::bind(&PFDigitalModule::handle_interrupt, this),
                 zmqpp::poller::poll_pri | zmqpp::poller::poll_error);
}

void PFDigitalModule::run()
{
    while (is_running_)
    {
        auto itr_transform =
            [](const PFDigitalPin &p) -> std::chrono::system_clock::time_point {
            return p.next_update();
        };

        auto timeout = Tools::compute_timeout(
            boost::make_transform_iterator(gpios_.begin(), itr_transform),
            boost::make_transform_iterator(gpios_.end(), itr_transform));
        reactor_.poll(timeout);
        for (auto &gpio_pin : gpios_)
        {
            if (gpio_pin.next_update() < std::chrono::system_clock::now())
                gpio_pin.update();
        }
    }
}

void PFDigitalModule::handle_interrupt()
{
    // get interrupt state.
    std::array<char, 64> buffer;
    ssize_t ret;

    ret = ::read(interrupt_fd_, &buffer[0], buffer.size());
    ASSERT_LOG(ret >= 0,
               "Reading on interrupt_fd gave unexpected return value: " << ret);
    ret = ::lseek(interrupt_fd_, 0, SEEK_SET);
    ASSERT_LOG(ret >= 0,
               "Lseeking on interrupt_fd gave unexpected return value: " << ret);

    uint8_t states = pifacedigital_read_reg(0x11, 0);
    for (int i = 0; i < 8; ++i)
    {
        if (((states >> i) & 0x01) == 0)
        {
            // signal interrupt if needed (ie the pin is registered in config)
            std::string gpio_name;
            if (get_input_pin_name(gpio_name, i))
            {
                bus_push_.send(zmqpp::message()
                               << std::string("S_INT:" + gpio_name));
            }
        }
    }
}

bool PFDigitalModule::get_input_pin_name(std::string &dest, int idx)
{
    for (auto &gpio : gpios_)
    {
        if (gpio.gpio_no_ == idx && gpio.direction_ == PFDigitalPin::Direction::In)
        {
            dest = gpio.name_;
            return true;
        }
    }
    return false;
}

void PFDigitalModule::process_config(const boost::property_tree::ptree &cfg)
{
    boost::property_tree::ptree module_config = cfg.get_child("module_config");

    for (auto &node : module_config.get_child("gpios"))
    {
        boost::property_tree::ptree gpio_cfg = node.second;

        std::string gpio_name      = gpio_cfg.get_child("name").data();
        int gpio_no                = std::stoi(gpio_cfg.get_child("no").data());
        std::string gpio_direction = gpio_cfg.get_child("direction").data();
        bool gpio_value            = gpio_cfg.get<bool>("value", false);

        INFO("Creating GPIO " << gpio_name << ", with no " << gpio_no
                              << ". direction = " << gpio_direction);

        PFDigitalPin pin(ctx_, gpio_name, gpio_no,
                         gpio_direction == "in" ? PFDigitalPin::Direction::In
                                                : PFDigitalPin::Direction::Out,
                         gpio_value);

        if (gpio_direction != "in" && gpio_direction != "out")
            throw GpioException("Direction (" + gpio_direction + ") is invalid");
        gpios_.push_back(std::move(pin));
        utils_->config_checker().register_object(gpio_name,
                                                 ConfigChecker::ObjectType::GPIO);
    }
}
