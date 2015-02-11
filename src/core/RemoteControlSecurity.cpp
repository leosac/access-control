#include "core/RemoteControlSecurity.hpp"

using namespace Leosac;

RemoteControlSecurity::RemoteControlSecurity(const boost::property_tree::ptree &cfg) :
        cfg_(cfg)
{

}

bool RemoteControlSecurity::allow_request(const std::string &req, const std::string &user_pubkey)
{
    return true;
}
