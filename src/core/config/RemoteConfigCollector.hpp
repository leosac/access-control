/*
    Copyright (C) 2014-2015 Islog

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

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <zmqpp/zmqpp.hpp>
#include <boost/property_tree/ptree.hpp>

namespace Leosac
{
    /**
    * This class provides an API to collect the configuration of a remote Leosac unit.
    *
    * It is used by the Remote Control code in order to retrieve the remote configuration
    * when handling a SYNC_FROM command.
    *
    * The current behavior of the object is a BLOCKING collection. This is not the best
    * way, but it is simpler and with proper timeout to avoid infinite recv() call, it works.
    */
    class RemoteConfigCollector
    {
    public:
        /**
        * Construct a new RemoteConfigCollector, this object will work for a one-time config
        * collection.
        *
        * @param ctx the zeromq context to create a socket.
        * @param remote_endpoint the endpoint (tcp://ip:port) of the unit we want to collect config.
        * @param remote_pk the public key of the remote. Provide some security, and avoid connecting to a unwanted server.
        */
        RemoteConfigCollector(zmqpp::context_t &ctx,
                const std::string &remote_endpoint,
                const std::string &remote_pk);
        virtual ~RemoteConfigCollector() = default;

        RemoteConfigCollector(const RemoteConfigCollector &) = delete;
        RemoteConfigCollector(RemoteConfigCollector &&) = delete;
        RemoteConfigCollector &operator=(const RemoteConfigCollector &) = delete;
        RemoteConfigCollector &operator=(RemoteConfigCollector &&) = delete;

        using ModuleConfigMap = std::map<std::string, boost::property_tree::ptree>;


        /**
        * Fetch the complete remote configuration.
        * If this call returns true, the various parts of the configuration
        * are available to you through this class' public methods.
        *
        * This function does not throw.
        *
        * If this function fails, and error_str is not nullptr, we store the
        * reason of the failure here.
        */
        bool fetch_config(std::string *error_str) noexcept;

        /**
        * Return the list of modules that are loaded on the remote host.
        * Make sure that fetch_config() was called and succeed first.
        */
        const std::list<std::string> &modules_list() const noexcept;

        const ModuleConfigMap &modules_config() const noexcept;

        const boost::property_tree::ptree &module_config(const std::string &name) const;

    private:
        /**
        * Send the GENERAL_CONFIG command to the remote, and wait for response.
        */
        bool fetch_general_config();

        /**
        * Sends the MODULE_LIST command.
        */
        bool fetch_module_list();

        /**
        * Sends the MODULE_CONFIG command for the module whose name is `module_name`.
        */
        bool fetch_module_config(const std::string &module_name);

        /**
        * Fetch the conf for all modules (relying on fetch_module_config()).
        */
        bool fetch_modules_config();

        std::string remote_endpoint_;

        /**
        * Poll on the socket.
        */
        zmqpp::poller_t poller_;
        zmqpp::socket_t sock_;

        long mstimeout_;

        /**
        * Map module name to their config tree.
        * The maps start empty and is filled by fetch_module_config();
        */
        ModuleConfigMap config_map_;
        boost::property_tree::ptree general_config_;
        std::list<std::string> module_list_;

        // those 2 boolean are here to enforce the correct use of the object.
        // Call fetch_config() that access various config item.

        bool first_call_;
        bool succeed_;
    };
}