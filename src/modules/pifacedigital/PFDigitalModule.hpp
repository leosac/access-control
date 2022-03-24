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

#include "PFDigitalPin.hpp"
#include "modules/BaseModule.hpp"
#include "tools/service/ServiceRegistry.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/property_tree/ptree.hpp>
#include <modules/websock-api/WSHelperThread.hpp>
#include <zmqpp/reactor.hpp>
#include <zmqpp/socket.hpp>

namespace Leosac
{
namespace Module
{
/**
* Provide support for the piface digital device.
 *
 * This module can be started on a device that is not compatible
 * with a Piface device just to be able to configure Piface backed
 * GPIO.
 *
 * If database is enabled, this module use one helper thread to listen
 * to websocket request and to perform database operation.
*
* @see @ref mod_piface_main for documentation
*/
namespace Piface
{

class PFDigitalModule;
/**
* Some ~const parameter that are required
* to process websocket requests.
*/
struct ModuleParameters
{
    bool degraded_mode;
};

class WSHelperThread : public WebSockAPI::BaseModuleSupportThread<ModuleParameters>
{
  public:
    explicit WSHelperThread(const CoreUtilsPtr &core_utils)
        : BaseModuleSupportThread(core_utils, {})
    {
    }

    /**
     * Implements the "pfdigital.test_output_pin" API call.
     */
    void test_output_pin(const UUID &gpio_id);

    void unregister_ws_handlers(WebSockAPI::Service &ws_service) override;

  private:
    void register_ws_handlers(WebSockAPI::Service &ws_service) override;
};

/**
* Main class for the piface digital module.
*/
class PFDigitalModule : public BaseModule
{
  public:
    PFDigitalModule(zmqpp::context &ctx, zmqpp::socket *module_manager_pipe,
                    const boost::property_tree::ptree &config, CoreUtilsPtr utils);


    ~PFDigitalModule();

    /**
    * Module's main loop.
    */
    virtual void run() override;

  private:
    /**
    * An interrupt was triggered. Lets handle it.
    */
    void handle_interrupt();

    /**
    * Process the XML configuration, preparing configured GPIO pin.
    */
    void process_xml_config(const boost::property_tree::ptree &cfg);

    /**
     * Process configuration.
     */
    void process_config();

    /**
     * Create / update database schema for the module
     */
    void setup_database();

    void load_config_from_database();

    /**
    * Socket to push event to the bus.
    */
    zmqpp::socket bus_push_;

    /**
    * GPIO vector
    */
    std::vector<PFDigitalPin> gpios_;

    /**
     * Retrieve the (user-given) name of the pin and store it in dest.
     * @param dest string to store the name into
     * @param idx The ping number, starting at 0 for the first one.
     * @param hw_addr The underlying hardware address of the pifacedigital.
     * @return True if we found the pin, false otherwise.
     */
    bool get_input_pin_name(std::string &dest, int idx, uint8_t hw_addr);

    /**
    * File descriptor of the PIN that triggers interrupts. This is card and will not
    * change.
    */
    int interrupt_fd_;

    /**
     * Support thread for processing websocket requests.
     */
    WSHelperThread ws_helper_thread_;

    /**
     * True if we are running in "degraded" mode (ie, not on a device
     * that support the PifaceDigital).
     */
    bool degraded_mode_;
};
}
}
}
