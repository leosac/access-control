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

#pragma once

#include "LibgpiodPin.hpp"
#include "LibgpiodConfig.hpp"
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
* the Linux Kernel libgpiod interface.
*
* @see @ref mod_libgpiod_main for end-user documentation.
*/
namespace Libgpiod
{
class LibgpiodPin;

class LibgpiodConfig;

/**
* Handle GPIO management over libgpiod.
* @see @ref mod_libgpiod_user_config for configuration information.
*/
class LibgpiodModule : public BaseModule
{
  public:
    LibgpiodModule(zmqpp::context &ctx, zmqpp::socket *module_manager_pipe,
                    const boost::property_tree::ptree &config, CoreUtilsPtr utils);

    ~LibgpiodModule();

    LibgpiodModule(const LibgpiodModule &) = delete;

    LibgpiodModule &operator=(LibgpiodModule &&) = delete;

    /**
    * Write the message eon the bus.
    * This is intended for use by the LibgpiodPin
    */
    void publish_on_bus(zmqpp::message &msg);

    /**
    * Retrieve the config object.
    */
    std::shared_ptr<LibgpiodConfig> general_config() const;

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
    * Socket to write the bus.
    */
    zmqpp::socket bus_push_;

    /**
    * Vector of underlying pin object
    */
    std::vector<std::shared_ptr<LibgpiodPin>> gpios_;

    /**
    * General configuration for module
    */
    std::shared_ptr<LibgpiodConfig> general_cfg_;
};
}
}
}
