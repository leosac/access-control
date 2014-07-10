/**
 * \file networkconfig.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief NetworkConfig class implementation
 */

#include "networkconfig.hpp"

#include <sstream>

#include "tools/log.hpp"
#include "tools/unixshellscript.hpp"
#include "exception/scriptexception.hpp"

void NetworkConfig::serialize(ptree& node)
{
    node.put<bool>("<xmlattr>.enabled", _enabled);
    node.put<std::string>("interface", _interface);
    node.put<bool>("dhcp", _dhcpEnabled);
    node.put<std::string>("ip", _ip);
    node.put<std::string>("netmask", _netmask);
    node.put<std::string>("default_ip", _defaultIp);
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

template<typename T>
static std::string toCmdLine(T value)
{
    std::ostringstream oss;

    oss << value;
    return (oss.str());
}

template<typename T, typename... Targs>
static std::string toCmdLine(T value, Targs... args)
{
    if (sizeof...(args) > 0)
        return (toCmdLine(value) + ' ' + toCmdLine(args...));
}

void NetworkConfig::reload()
{
    UnixShellScript builder(UnixFs::getCWD() + '/' + "build_ipconfig.sh");
    UnixShellScript apply(UnixFs::getCWD() + '/' + "load_ipconfig.sh");

    if (!_enabled)
        return;

    builder.run(toCmdLine(_dhcpEnabled, "mylamaisded", _interface, _ip, _netmask, "1&>/dev/null"));
    if (!builder.getOutput().empty())
        throw (ScriptException(builder.getOutput()));

    apply.run(toCmdLine("mylamaisded", _interface, "1&>/dev/null"));
    if (!apply.getOutput().empty())
        throw (ScriptException(apply.getOutput()));
}

void NetworkConfig::setEnabled(bool state)
{
    _enabled = state;
}

void NetworkConfig::resetDefaultIp()
{
    _ip = _defaultIp;
}
