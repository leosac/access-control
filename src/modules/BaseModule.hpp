/*
    Copyright (C) 2014-2016 Islog

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

#include "LeosacFwd.hpp"
#include "core/config/ConfigChecker.hpp"
#include "core/config/ConfigManager.hpp"
#include "tools/gettid.hpp"
#include "tools/log.hpp"
#include <boost/property_tree/ptree.hpp>
#include <zmqpp/zmqpp.hpp>

extern "C" {
const char *get_module_name();
}

namespace Leosac
{
namespace Module
{
/**
* This function template is a helper wrt writing the
* `start_module` function for every module.
*
* This helper allows you to create a 1 line `start_module` function.
*/
template <typename UserModule>
bool start_module_helper(zmqpp::socket *pipe, boost::property_tree::ptree cfg,
                         zmqpp::context &zmq_ctx, CoreUtilsPtr utils)
{
    try
    {
        UserModule module(zmq_ctx, pipe, cfg, utils);
        INFO("Module " << get_module_name()
                       << " is now initialized. Thread id = " << Leosac::gettid());
        pipe->send(zmqpp::signal::ok);

        module.run();
    }
    catch (const std::exception &e)
    {
        ERROR("An exception occurred in module code. Leosac will terminate."
              << std::endl
              << "Message: " << e.what() << std::endl
              << "Module: " << get_module_name());
        std::terminate();
    }
    INFO("Module " << get_module_name() << " has now terminated.");
    return true;
}

/**
* Base class for module implementation.
*
* It provide a base class for writing module code. The base class
* implements `run()` as a main loop and use a reactor to handle socket
* when they become available for reading. When implementing a module using
* this class you're supposed to use that reactor and register your socket.
* In case you need to override `run()`, remember to poll on the reactor,
* or to watch the `pipe_` on way or another.
*
* It use a reactor to poll on the `pipe` socket that connect the module back to the
* module manager.
*
* A module shall have a control socket and it must be a REP socket and
* bound to inproc://module-${MODULE_NAME}
*
* For replication and ease of management, command can be written on the module's
* control socket and must
* be implemented in the module (unless it is implemented by the BaseModule class).
*
* Commands:
*   DUMP_CONFIG: serialize the current module config.
*              : Parameter: "0" -> as a ptree
*                           "1" -> as a xml tree
*
* It is possible some module require additional configuration file. If that's the
* case,
* those this shall be copied in the response.
* Response may look like this:
* + XML (or ptree encoded by boost serialization)
* + FileName1
* + ContentOfFilename1
* + Filename2
* + Content of Filename2
*
*
* @note This class is here to help reduce code duplication. It is NOT mandatory to
* inherit from this base class
* to implement a module. However, it may help.
*/
class BaseModule
{
  public:
    /**
    * Constructor of BaseModule. It will register the pipe_ to reactor_.
    */
    BaseModule(zmqpp::context &ctx, zmqpp::socket *pipe,
               const boost::property_tree::ptree &cfg, CoreUtilsPtr utils);

    virtual ~BaseModule() = default;

    /**
    * This is the main loop of the module. It should only exit when receiving
    * `signal::stop` on its `pipe_`
    * socket. This is completely handled by this base class.
    * In case you override this, make sure you watch the `pipe_` socket.
    */
    virtual void run();

  protected:
    /**
    * The base class register the `pipe_` socket to its `reactor_` so that this
    * function
    * is called when the `pipe_` is available from reading.
    * This implementation switch `is_running_` to false.
    */
    virtual void handle_pipe();

    /**
    * Handle called when a message on the module's control socket arrives.
    */
    virtual void handle_control();

    /**
    * Dump additional configuration (for example module specific
    * config file).
    *
    * If your module has this kind of file, you may override
    * this method to provide correct dump of the config.
    *
    * @note Unless reimplemented this method does nothing.
    * @note See DUMP_CONFIG specs for additional config file.
    *
    * @param out MUST not be null.
    */
    virtual void dump_additional_config(zmqpp::message *out) const;

    /**
    * Fills a message with the module's configuration information.
    *
    * This method calls the `dump_additional_config()` method.
    * @param out_msg MUST not be null.
    */
    void dump_config(ConfigManager::ConfigFormat fmt, zmqpp::message *out_msg) const;

    /**
     * An helper that checks configuration the existence of some objects.
     * Based on whether or not strict mode is enable, it prints a warning or
     * prints an error and assert.
     */
    void config_check(const std::string &obj_name, ConfigChecker::ObjectType type);

    /**
     * An helper that checks configuration the existence of some objects.
     * Based on whether or not strict mode is enable, it prints a warning or
     * prints an error and assert.
     */
    void config_check(const std::string &obj_name);

    /**
    * A reference to the ZeroMQ context in case you need it to create additional
    * socket.
    */
    zmqpp::context &ctx_;

    /**
    * A reference to the pair socket that link back to the module manager.
    */
    zmqpp::socket &pipe_;

    /**
    * The configuration tree passed to the `start_module` function.
    */
    boost::property_tree::ptree config_;

    /**
     * Pointer to the core utils, which gives access to scheduler and others.
     */
    CoreUtilsPtr utils_;

    /**
    * Boolean indicating whether the main loop should run or not.
    */
    bool is_running_;

    /**
    * Control REP socket.
    */
    zmqpp::socket control_;

    /**
    * The reactor object we poll() on in the main loop. Register additional socket/fd
    * here if you need to.
    */
    zmqpp::reactor reactor_;

    std::string name_;
};
}
}
