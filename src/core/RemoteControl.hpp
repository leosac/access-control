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

#include "RemoteControlSecurity.hpp"
#include "core/config/ConfigManager.hpp"
#include <boost/property_tree/ptree_fwd.hpp>
#include <zmqpp/zmqpp.hpp>

namespace Leosac
{

class Kernel;

/**
* This class handle the remote control of leosac.
*
* It's responsibility is to expose a secure socket on the network and filter
* request from the world, passing legit message to the rest of the application.
*
* @see @ref remote_control_main For an overview of the Remote Control functionality.
*/
class RemoteControl
{
  public:
    RemoteControl(zmqpp::context &ctx, Kernel &kernel,
                  const boost::property_tree::ptree &cfg);

    void update();

  private:
    /**
    * Extract and verify content from user-message and call implementation.
    *
    * Returning false means the source message was malformed.
    */
    bool handle_module_config(zmqpp::message *msg_in, zmqpp::message *msg_out);

    /**
    * Extract and verify content from message and call implementation.
    *
    * Returning false means the source message was malformed.
    */
    bool handle_module_list(zmqpp::message *msg_in, zmqpp::message *msg_out);

    /**
    * Extract and verify content from message and call `sync_from()`
    *
    * Returning false means the source message was malformed.
    */
    bool handle_sync_from(zmqpp::message *msg_in, zmqpp::message *msg_out);

    /**
    * Save the current configuration to disk.
    *
    * Returning false means the source message was malformed.
    */
    bool handle_save(zmqpp::message *msg_in, zmqpp::message *msg_out);

    /**
    * Command handler for GENERAL_CONFIG.
    *
    * Returning false means the source message was malformed.
    */
    bool handle_general_config(zmqpp::message *msg_in, zmqpp::message *msg_out);

    /**
     * Command handler for CONFIG_VERSION command.
     *
     * It shall return the current version of the configuration.
     * This is used for synchronization purpose.
     */
    bool handle_config_version(zmqpp::message *msg_in, zmqpp::message *msg_out);

    /**
    * Implements the module list command.
    *
    * It fetch the list of module from the module manager object.
    */
    void module_list(zmqpp::message *message_out);

    /**
    * Implements the MODULE_CONFIG command.
    */
    void module_config(const std::string &module,
                       ConfigManager::ConfigFormat cfg_format,
                       zmqpp::message *message_out);

    /**
    * Implements GLOBAL_CONFIG API call.
    *
    * This returns configuration information unrelated to modules.
    */
    void general_config(ConfigManager::ConfigFormat cfg_format,
                        zmqpp::message *msg_out);

    /**
    * Register by core and called when message arrives.
    */
    void handle_msg();

    void process_config(const boost::property_tree::ptree &cfg);

    /**
    * Build the list of modules (their name) running on the remote host.
    * We issue the MODULE_LIST command in order to do that.
    *
    * @return false if we can't get this list.
    */
    bool gather_remote_module_list(zmqpp::socket &s,
                                   std::list<std::string> &remote_modules);

    /**
    * After sending a bunch of MODULE_CONFIG command, wait for reply.
    *
    * Update the configuration manager object with the newly received configuration.
    * Also update additional configuration if needed.
    *
    * If receiving command timeout (>3s) return false;
    */
    bool receive_remote_config(zmqpp::socket &s, std::map<std::string, bool> &cfg);

    /**
    * Will retrieve the config for a remote leosac unit.
    *
    * The socket shall be already connected to the remote endpoint.
    *
    * It will populate the stop_list and start_list with a list of to respectively
    * stop and start.
    */
    bool gather_remote_config(zmqpp::socket &s, std::list<std::string> &start_list,
                              std::list<std::string> &stop_list);

    Kernel &kernel_;

    /**
    * Public ROUTER
    */
    zmqpp::socket socket_;

    /**
    * z85 encoded public curve key
    */
    std::string public_key_;

    /**
    * z85 encoded private curve key
    */
    std::string secret_key_;

    zmqpp::auth auth_;
    zmqpp::context &context_;

    // Function is not really void (), we use placeholder and bind.
    using CommandHandlerMap =
        std::map<std::string, std::function<bool(zmqpp::message *msg_in,
                                                 zmqpp::message *msg_out)>>;

    CommandHandlerMap command_handlers_;

    /**
    * Object to check remote user permission before processing their request.
    */
    RemoteControlSecurity security_;

    std::string current_client_idt_;

    // Allow kernel full access to this class.
    friend class Kernel;
};
}
