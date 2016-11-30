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

#pragma once

#include "SysFSGPIOPin.hpp"
#include "SysFsGpioConfig.hpp"
#include <boost/property_tree/ptree.hpp>
#include <modules/BaseModule.hpp>
#include <zmqpp/reactor.hpp>
#include <zmqpp/socket.hpp>

namespace Leosac
{
namespace Module
{
/**
* Namespace for the module that implements GPIO support using
* the Linux Kernel sysfs interface.
*
* @see @ref mod_sysfsgpio_main for end-user documentation.
*/
namespace SysFsGpio
{
class SysFsGpioPin;

class SysFsGpioConfig;

/**
* Handle GPIO management over sysfs.
* @see @ref mod_sysfsgpio_user_config for configuration information.
*/
class SysFsGpioModule : public BaseModule
{
  public:
    SysFsGpioModule(zmqpp::context &ctx, zmqpp::socket *module_manager_pipe,
                    const boost::property_tree::ptree &config, CoreUtilsPtr utils);

    ~SysFsGpioModule();

    SysFsGpioModule(const SysFsGpioModule &) = delete;

    SysFsGpioModule &operator=(SysFsGpioModule &&) = delete;

    /**
    * Write the message eon the bus.
    * This is intended for use by the SysFsGpioPin
    */
    void publish_on_bus(zmqpp::message &msg);

    /**
    * Retrieve a reference to the config object.
    */
    const SysFsGpioConfig &general_config() const;

    virtual void run() override;


  private:
    /**
    * Process the configuration, preparing configured GPIO pin.
    */
    void process_config(const boost::property_tree::ptree &cfg);

    /**
    * General configuration (file paths, etc).
    */
    void process_general_config();

    /**
    * Write to "gpio_export_path" so the kernel export the socket to sysfs.
    */
    void export_gpio(int gpio_no);

    /**
    * Socket to write the bus.
    */
    zmqpp::socket bus_push_;

    /**
    * Vector of underlying pin object
    */
    std::vector<SysFsGpioPin *> gpios_;

    /**
    * General configuration for module
    */
    SysFsGpioConfig *general_cfg_;
};
}
}
}
