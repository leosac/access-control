/**
 * \file networkconfig.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief NetworkConfig class implementation
 */

#include "networkconfig.hpp"

#include "tools/log.hpp"
#include "tools/unixshellscript.hpp"
#include "exception/scriptexception.hpp"

const std::string NetworkConfig::NetCfgFile = "interfaces";

NetworkConfig::NetworkConfig()
:   _enabled(false),
    _dhcpEnabled(false)
{}

void NetworkConfig::serialize(ptree& node)
{
    node.put<bool>("<xmlattr>.enabled", _enabled);
    node.put<std::string>("interface", _interface);
    node.put<bool>("dhcp", _dhcpEnabled);
    node.put<std::string>("ip", _ip);
    node.put<std::string>("netmask", _netmask);
    node.put<std::string>("default_ip", _defaultIp);
    _enabled = false;
}

void NetworkConfig::deserialize(const ptree& node)
{
    _enabled = node.get<bool>("<xmlattr>.enabled");
    _interface = node.get<std::string>("interface");
    _dhcpEnabled = node.get<bool>("dhcp");
    _netmask = node.get<std::string>("netmask");
    _defaultIp = node.get<std::string>("default_ip");
    _ip = node.get<std::string>("ip", _defaultIp);

    LOG() << "NetworkSettings:";
    LOG() << "enabled=" << _enabled;
    LOG() << "dhcp=" << _dhcpEnabled;
    LOG() << "ip=" << _ip;
    LOG() << "netmask=" << _netmask;
    LOG() << "default=" << _defaultIp;
}

void NetworkConfig::reload()
{
    UnixShellScript builder(UnixFs::getCWD() + '/' + "build_ipconfig.sh");
    UnixShellScript apply(UnixFs::getCWD() + '/' + "load_ipconfig.sh");

    if (!_enabled)
        return;

    builder.run(UnixShellScript::toCmdLine(_dhcpEnabled, NetCfgFile, _interface, _ip, _netmask, "1&>/dev/null"));
    if (!builder.getOutput().empty())
        throw (ScriptException(builder.getOutput()));

    apply.run(UnixShellScript::toCmdLine(NetCfgFile, _interface, "1&>/dev/null"));
    if (!apply.getOutput().empty())
        LOG() << "ScriptOutput:\n" << apply.getOutput() << "\n";
    LOG() << "JUST LOAD IFCONFIG CONFIGURATION";
}

void NetworkConfig::setEnabled(bool state)
{
    _enabled = state;
}

void NetworkConfig::setDHCP(bool enabled)
{
    LOG() << "DHCP";
    if (_dhcpEnabled != enabled)
    {
        LOG() << "DHCP set to" << enabled;
        _dhcpEnabled = enabled;
        reload();
    }
}

void NetworkConfig::setCustomIP(bool enabled)
{
    LOG() << "CustomIP set to" << enabled;
    _ip = _defaultIp;
}
