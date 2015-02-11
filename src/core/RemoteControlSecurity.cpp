#include <tools/log.hpp>
#include <tools/XmlPropertyTree.hpp>
#include <boost/algorithm/string/join.hpp>
#include "core/RemoteControlSecurity.hpp"

using namespace Leosac;

RemoteControlSecurity::RemoteControlSecurity(const boost::property_tree::ptree &cfg) :
        cfg_(cfg)
{
    process_config();
}

void RemoteControlSecurity::process_config()
{
    const auto &security_details = cfg_.get_child_optional("security");

    if (security_details)
    {

        for (const auto &entry : (*security_details))
        {
            assert(entry.first == "map");
            process_security_entry(entry.second);
        }
    }
}

bool RemoteControlSecurity::allow_request(const std::string &user_pubkey, const std::string &req)
{
    if (default_permissions_.count(user_pubkey) == 0 || permissions_.count(user_pubkey) == 0)
    {
        WARN("Received command from " << user_pubkey << " but no permission information for this user. Denying.");
        return false;
    }

    const auto &cmds = permissions_[user_pubkey];
    if (default_permissions_[user_pubkey]) // check cmd is not explicitely denied
        return std::find(cmds.begin(), cmds.end(), req) == cmds.end();
    else
        return std::find(cmds.begin(), cmds.end(), req) != cmds.end();
}

void RemoteControlSecurity::process_security_entry(const boost::property_tree::ptree &entry)
{
    //DEBUG(Tools::propertyTreeToXml(entry));
    const std::string &pk = entry.get<std::string>("pk");
    bool default_access   = entry.get<bool>("default", false);

    default_permissions_[pk] = default_access;
    // all child of entry, except "pk" and "default" are command name.
    for (const auto &c : entry)
    {
        if (c.first == "pk" || c.first == "default")
            continue;
        std::string cmd_name = c.first;
        bool allowed = c.second.get_value<bool>();

        // if user has default access granted, and this command is granted too,
        // we do not store it, because we store only denied command.
        if (default_access != allowed)
            permissions_[pk].push_back(cmd_name);
    }

    INFO("Processed configuration for remote user. \n\t "
            << "Public key: " << pk << "\n\t "
            << "Default permission: " << default_access << "\n\t "
            << (default_access ? "Disabled command: " : "Enabled commands: ")
            << boost::algorithm::join(permissions_[pk], ", "));
}
