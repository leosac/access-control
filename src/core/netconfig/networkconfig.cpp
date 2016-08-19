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

/**
 * \file networkconfig.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief NetworkConfig class implementation
 */

#include "networkconfig.hpp"
#include "core/kernel.hpp"
#include "tools/log.hpp"
#include "tools/unixshellscript.hpp"
#include <boost/property_tree/ptree_fwd.hpp>

using namespace Leosac::Tools;
using namespace Leosac;

NetworkConfig::NetworkConfig(const Kernel &k, const boost::property_tree::ptree &cfg)
    : config_(cfg)
    , _enabled(false)
    , _dhcpEnabled(false)
    , kernel_(k)
{
    _enabled = cfg.get<bool>("enabled", false);

    if (_enabled)
    {
        _interface   = cfg.get<std::string>("interface");
        _dhcpEnabled = cfg.get<bool>("dhcp");
        _netmask     = cfg.get<std::string>("netmask");
        _defaultIp   = cfg.get<std::string>("default_ip");
        _ip          = cfg.get<std::string>("ip", _defaultIp);
        _gateway     = cfg.get<std::string>("gateway");

        INFO("Network settings:" << std::endl
                                 << '\t' << "enabled=" << _enabled << std::endl
                                 << '\t' << "dhcp=" << _dhcpEnabled << std::endl
                                 << '\t' << "ip=" << _ip << std::endl
                                 << '\t' << "netmask=" << _netmask << std::endl
                                 << '\t' << "gateway=" << _gateway << std::endl
                                 << '\t' << "default=" << _defaultIp);
    }
    else
        INFO("Network configuration disabled. Will not touch system setting.");
}

void NetworkConfig::reload()
{
    UnixShellScript builder(kernel_.script_directory() + "build_ipconfig.sh");
    UnixShellScript apply(kernel_.script_directory() + "load_ipconfig.sh");

    if (!_enabled)
        return;

    builder.run(UnixShellScript::toCmdLine(_dhcpEnabled, NetCfgFile, _interface, _ip,
                                           _netmask, _gateway, "1&>/dev/null"));
    if (!builder.getOutput().empty())
        throw(ScriptException(builder.getOutput()));

    apply.run(UnixShellScript::toCmdLine(NetCfgFile, _interface, "1&>/dev/null"));
    if (!apply.getOutput().empty())
        INFO("ScriptOutput:\n" << apply.getOutput() << "\n");
    INFO("JUST LOADED IFCONFIG CONFIGURATION");
}

void NetworkConfig::setEnabled(bool state)
{
    _enabled = state;
}

void NetworkConfig::setDHCP(bool enabled)
{
    if (_dhcpEnabled != enabled)
    {
        INFO("DHCP set to" << enabled);
        _dhcpEnabled = enabled;
        reload();
    }
}

void NetworkConfig::setCustomIP(bool enabled)
{
    INFO("CustomIP set to" << enabled);
    _ip = _defaultIp;
}
