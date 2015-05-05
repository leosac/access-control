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

#include <zmqpp/curve.hpp>
#include <exception/ExceptionsTools.hpp>
#include <tools/log.hpp>
#include <tools/XmlPropertyTree.hpp>
#include "core/config/RemoteConfigCollector.hpp"
#include "core/config/ConfigManager.hpp"

using namespace Leosac;

RemoteConfigCollector::RemoteConfigCollector(zmqpp::context_t &ctx,
        std::string const &remote_endpoint,
        std::string const &remote_pk) :
        remote_endpoint_(remote_endpoint),
        sock_(ctx, zmqpp::socket_type::dealer),
        mstimeout_(1500),
        first_call_(true),
        succeed_(false)
{
    auto kp = zmqpp::curve::generate_keypair();
    sock_.set(zmqpp::socket_option::curve_secret_key, kp.secret_key);
    sock_.set(zmqpp::socket_option::curve_public_key, kp.public_key);
    sock_.set(zmqpp::socket_option::curve_server_key, remote_pk);
    sock_.set(zmqpp::socket_option::linger, 0);
    poller_.add(sock_);
}

bool RemoteConfigCollector::fetch_config(std::string *error_str) noexcept
{
    assert(first_call_);
    first_call_ = false;

    try
    {
        sock_.connect(remote_endpoint_);

        if (!fetch_general_config())
        {
            WARN("Error fetching general configuration of remote Leosac (" << remote_endpoint_ << ")");
            if (error_str)
                *error_str = "Fetching general remote configuration failed.";
            return false;
        }
        if (!fetch_module_list())
        {
            WARN("Error fetching module list from remote Leosac (" << remote_endpoint_ << ")");
            if (error_str)
                *error_str = "Fetching remote module list failed.";
            return false;
        }
        if (!fetch_modules_config())
        {
            WARN("Error fetching modules configuration from remote Leosac (" << remote_endpoint_ << ")");
            if (error_str)
                *error_str = "Fetching remote modules configuration failed.";
            return false;
        }
        succeed_ = true;
        return true;
    }
    catch (std::exception &e)
    {
        print_exception(e);
        if (error_str)
            *error_str = std::string("Exception occured: ") + e.what();
    }
    return false;
}

bool RemoteConfigCollector::fetch_general_config()
{
    zmqpp::message msg;

    msg << "GENERAL_CONFIG" << ConfigManager::ConfigFormat::BOOST_ARCHIVE;
    sock_.send(msg);
    poller_.poll(mstimeout_);

    if (poller_.has_input(sock_))
    {
        sock_.receive(msg);
        if (msg.remaining() == 2)
        {
            std::string tmp;
            msg >> tmp;
            assert(tmp == "OK");
            msg >> tmp;
            if (Tools::boost_text_archive_to_ptree(tmp, general_config_))
                return true;
        }
    }
    return false;
}

bool RemoteConfigCollector::fetch_module_list()
{
    sock_.send("MODULE_LIST");

    poller_.poll(mstimeout_);
    if (poller_.has_input(sock_))
    {
        zmqpp::message msg;
        sock_.receive(msg);

        while (msg.remaining())
        {
            std::string tmp;
            msg >> tmp;
            module_list_.push_back(tmp);
        }
        return true;
    }
    return false;
}

bool RemoteConfigCollector::fetch_module_config(const std::string &module_name)
{
    zmqpp::message msg;

    msg << "MODULE_CONFIG" << module_name << ConfigManager::ConfigFormat::BOOST_ARCHIVE;
    sock_.send(msg);
    poller_.poll(mstimeout_);

    if (poller_.has_input(sock_))
    {
        sock_.receive(msg);
        if (msg.remaining() < 3)
            return false;

        std::string result;
        std::string config_str;
        std::string recv_module_name;

        msg >> result >> recv_module_name >> config_str;
        assert(result == "OK");
        assert(recv_module_name == module_name);

        // process additional file.
        if (msg.remaining() % 2 != 0)
        {
            ERROR("Msg has " << msg.remaining() << " remaining parts, but need a multiple of 2.");
            return false;
        }
        while (msg.remaining())
        {
            std::string file_name;
            std::string file_content;

            msg >> file_name >> file_content;
            additional_files_[module_name].push_back(std::make_pair(file_name, file_content));
        }

        // make sure the map is not empty event if there is no file.
        additional_files_[module_name];

        if (Tools::boost_text_archive_to_ptree(config_str, config_map_[module_name]))
            return true;
    }
    return false;
}

bool RemoteConfigCollector::fetch_modules_config()
{
    for (const auto &mod_name : module_list_)
    {
        if (!fetch_module_config(mod_name))
            return false;
    }
    return true;
}

const std::list<std::string> &RemoteConfigCollector::modules_list() const noexcept
{
    assert(succeed_);
    return module_list_;
}

const RemoteConfigCollector::ModuleConfigMap &RemoteConfigCollector::modules_config() const noexcept
{
    assert(succeed_);
    return config_map_;
}

const boost::property_tree::ptree &RemoteConfigCollector::module_config(const std::string &name) const
{
    assert(succeed_);
    auto itr = config_map_.find(name);
    if (itr != config_map_.end())
        return itr->second;
    assert(0);
    throw std::runtime_error("Code is broken: module " + name + " doesn't exist in this config map.");
}

const boost::property_tree::ptree &RemoteConfigCollector::general_config() const
{
    return general_config_;
}

RemoteConfigCollector::FileNameContentList const &RemoteConfigCollector::additional_files(const std::string module) const
{
    if (additional_files_.count(module))
        return additional_files_.at(module);
    assert(0);
    throw std::runtime_error("Module doesn't exist here.");
}
