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

#include "PFDigitalPin.hpp"
#include "modules/BaseModule.hpp"
#include "tools/service/ServiceRegistry.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/property_tree/ptree.hpp>
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
 * Thread to perform websocket related operation.
 *
 * This is an helper thread of the PFDigitalModule.
 */
class WSHelperThread
{
  public:
    /**
     * Some ~const parameter that are required
     * to process websocket requests.
     */
    struct ModuleParameters
    {
        bool degraded_mode;
    };

    explicit WSHelperThread()
    {
        thread_ = std::make_unique<std::thread>([this]() { run_io_service(); });
    }

    ~WSHelperThread()
    {
        clear_work();
        try
        {
            thread_->join();
        }
        catch (const std::exception &e)
        {
            ERROR("Failed to join WSHelperThread");
        }
    }

    void register_ws_handlers();

    void on_service_event(const service_event::Event &);

    /**
     * Set required parameters.
     *
     * Call this before calling register_ws_handlers();
     */
    void set_parameter(ModuleParameters param);

  private:
    void run_io_service();

    void clear_work()
    {
        work_ = nullptr;
    }

    bs2::scoped_connection service_event_listener_;
    std::unique_ptr<std::thread> thread_;
    boost::asio::io_service io_;
    std::unique_ptr<boost::asio::io_service::work> work_;
    ModuleParameters parameters_;
    std::mutex mutex_;
};

/**
* Main class for the piface digital module.
*/
class PFDigitalModule : public BaseModule
{
  public:
    PFDigitalModule(zmqpp::context &ctx, zmqpp::socket *module_manager_pipe,
                    const boost::property_tree::ptree &config, CoreUtilsPtr utils);

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
     * Explicitely deregister websocket handler.
     */
    void remove_ws_handlers();

    /**
     * Process configuration.
     */
    void process_config();

    void setup_database();

    /**
    * Socket to push event to the bus.
    */
    zmqpp::socket bus_push_;

    /**
    * GPIO vector
    */
    std::vector<PFDigitalPin> gpios_;

    /**
    * Should be removed someday...
    * store the name of the input pin with id = idx in dest.
    *
    * returns true if it was successful (pin exists), false otherwise.
    */
    bool get_input_pin_name(std::string &dest, int idx);

    /**
    * File descriptor of the PIN that triggers interrupts. This is card and will not
    * change.
    */
    int interrupt_fd_;

    WSHelperThread helper_thread_;

    /**
     * True if we are running in "degraded" mode (ie, not on a device
     * that support the PifaceDigital).
     */
    bool degraded_mode_;
};
}
}
}
