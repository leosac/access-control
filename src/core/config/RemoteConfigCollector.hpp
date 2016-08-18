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

#include <boost/property_tree/ptree.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <zmqpp/zmqpp.hpp>

namespace Leosac
{
/**
* This class provides an API to collect the configuration of a remote Leosac unit.
*
* This class is wrapped by the Tasks::FetchRemoteConfig object.
* The implementation is a BLOCKING collection (with timeout), which is why one should
* use the Task object that wraps the collection.
*
* #### Consistency concerns (and solutions):
*
* Fetching the remote configuration requires multiple messages: 1 for the global
* configuration, and one per modules, at least. There is no "remote locking", which
* means that it is possible for the remote configuration to change while we are
* fetching it.
*
* Fortunately, there is a global `version` field that represents the version of the
* whole configuration. This number can only increase. What we do is similar to
* Optimistic Concurrency Control: we fetch the configuration version once before
* retrieving the configuration, then we fetch it again when we are done. If the
* number is the same, it means that the configuration didn't change.
*/
class RemoteConfigCollector
{
  public:
    /**
    * Construct a new RemoteConfigCollector, this object will work for a one-time
    * config
    * collection.
    *
    * @param ctx the zeromq context to create a socket.
    * @param remote_endpoint the endpoint (tcp://ip:port) of the unit we want to
    * collect config.
    * @param remote_pk the public key of the remote. Provide some security, and avoid
    * connecting to a unwanted server.
    */
    RemoteConfigCollector(zmqpp::context_t &ctx, const std::string &remote_endpoint,
                          const std::string &remote_pk);
    virtual ~RemoteConfigCollector() = default;

    RemoteConfigCollector(const RemoteConfigCollector &) = delete;
    RemoteConfigCollector(RemoteConfigCollector &&)      = delete;
    RemoteConfigCollector &operator=(const RemoteConfigCollector &) = delete;
    RemoteConfigCollector &operator=(RemoteConfigCollector &&) = delete;

    using ModuleConfigMap = std::map<std::string, boost::property_tree::ptree>;

    using FileNameContentList = std::list<std::pair<std::string, std::string>>;
    /**
    * Map module name to a list of (file_name, file_content)
    */
    using ModuleAdditionalFiles = std::map<std::string, FileNameContentList>;

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

    /**
    * Returns a reference to modules and their configuration.
    */
    const ModuleConfigMap &modules_config() const noexcept;

    /**
    * Returns the configuration for one specific module, identified by name.
    */
    const boost::property_tree::ptree &module_config(const std::string &name) const;

    /**
    * Returns the tree for the general configuration option.
    */
    const boost::property_tree::ptree &general_config() const;

    const FileNameContentList &additional_files(const std::string module) const;

    uint64_t remote_version() const;

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

    /**
     * Fetch the version of the remote configuration.
     * @param version store the version into the reference.
     */
    bool fetch_remote_config_version(uint64_t &version);

    std::string remote_endpoint_;
    std::string remote_pk_;

    /**
    * Poll on the socket.
    */
    zmqpp::poller_t poller_;
    zmqpp::socket_t sock_;

    long mstimeout_;

    uint64_t remote_version_;

    /**
    * Map module name to their config tree.
    * The maps start empty and is filled by fetch_module_config();
    */
    ModuleConfigMap config_map_;
    boost::property_tree::ptree general_config_;
    std::list<std::string> module_list_;
    ModuleAdditionalFiles additional_files_;

    // those 2 boolean are here to enforce the correct use of the object.
    // Call fetch_config() then access various config item.

    bool first_call_;
    bool succeed_;
};
}