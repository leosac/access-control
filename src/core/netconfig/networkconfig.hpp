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
public:
    explicit NetworkConfig() = default;
    ~NetworkConfig() = default;

    NetworkConfig(const NetworkConfig& other) = delete;
    NetworkConfig& operator=(const NetworkConfig& other) = delete;

public:
    virtual void    serialize(ptree& node) override;
    virtual void    deserialize(const ptree& node) override;

private:
    void    reload();

private:
    std::string _defaultIp;
    std::string _ip;
    bool        _dhcpEnabled;
};

#endif // NETWORKCONFIG_HPP
