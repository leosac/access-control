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

#include "MqttExternalServer.hpp"
#include "zmqpp/zmqpp.hpp"
#include <boost/property_tree/ptree.hpp>
#include <modules/BaseModule.hpp>

namespace Leosac
{
namespace Module
{

/**
* Provide support for Mqtt.
*
* @see @ref mod_mqtt_main for more information
*/
namespace Mqtt
{
class MqttConfig;
class MqttServerConfig;

/**
* This simply is the main class for the Wiegand module.
*/
class MqttModule : public BaseModule
{
  public:
    MqttModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                        const boost::property_tree::ptree &cfg, CoreUtilsPtr utils);

    ~MqttModule() override;

    virtual void run() override;

  private:
    /**
    * Create Mqtt server instances based on configuration.
    */
    void process_config();

    /**
     * Load the module configuration from the database.
     */
    void load_db_config();

    /**
     * Load the module configuration from the XML configuration
     * object.
     */
    void load_xml_config(const boost::property_tree::ptree &module_config);

    /**
    * Vector of mqtt connections managed by this module.
    */
    std::vector<MqttExternalServer> servers_;

    /**
     * Configuration object for the module.
     */
    std::unique_ptr<MqttConfig> mqtt_config_;
};
}
}
}
