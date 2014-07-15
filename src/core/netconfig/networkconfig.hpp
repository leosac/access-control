/**
 * \file networkconfig.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief NetworkConfig class declaration
 */

#ifndef NETWORKCONFIG_HPP
#define NETWORKCONFIG_HPP

#include "config/ixmlserializable.hpp"

class NetworkConfig : public IXmlSerializable
{
    static const std::string    NetCfgFile;

public:
    explicit NetworkConfig();
    ~NetworkConfig() = default;

    NetworkConfig(const NetworkConfig& other) = delete;
    NetworkConfig& operator=(const NetworkConfig& other) = delete;

public:
    virtual void    serialize(ptree& node) override;
    virtual void    deserialize(const ptree& node) override;

public:
    void    reload();
    void    setEnabled(bool state);
    void    setDHCP(bool enabled);
    void    setCustomIP(bool enabled);

private:
    bool        _enabled;
    std::string _interface;
    bool        _dhcpEnabled;
    std::string _netmask;
    std::string _ip;
    std::string _defaultIp;
};

#endif // NETWORKCONFIG_HPP
