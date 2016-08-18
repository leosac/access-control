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

/**
* \file networkconfig.hpp
* \author Thibault Schueller <ryp.sqrt@gmail.com>
* \brief NetworkConfig class declaration
*/

#include <boost/property_tree/ptree.hpp>

#ifndef NETWORKCONFIG_HPP
#define NETWORKCONFIG_HPP

namespace Leosac
{
class Kernel;

/**
* Class that helps configuring the network.
*
* @see @ref general_config_network for end-user documentation.
*/
class NetworkConfig
{
    static constexpr const char *NetCfgFile = "interfaces";

  public:
    explicit NetworkConfig(const Kernel &k, const boost::property_tree::ptree &cfg);

    ~NetworkConfig() = default;

    NetworkConfig(const NetworkConfig &other) = delete;

    NetworkConfig &operator=(const NetworkConfig &other) = delete;

  public:
    void reload();

    void setEnabled(bool state);

    void setDHCP(bool enabled);

    void setCustomIP(bool enabled);

  private:
    /**
    * Network configuration
    */
    boost::property_tree::ptree config_;

    bool _enabled;
    std::string _interface;
    bool _dhcpEnabled;
    std::string _netmask;
    std::string _ip;
    std::string _defaultIp;
    std::string _gateway;

    const Kernel &kernel_;
};
}

#endif // NETWORKCONFIG_HPP
