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
    /**
    * Class that helps configuring the network.
    *
    * @see @ref network_main for end-user documentation.
    */
    class NetworkConfig
    {
        static const std::string NetCfgFile;

    public:
        explicit NetworkConfig(const boost::property_tree::ptree &cfg);

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
    };
}

#endif // NETWORKCONFIG_HPP
