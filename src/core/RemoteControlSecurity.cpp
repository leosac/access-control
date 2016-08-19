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

#include "core/RemoteControlSecurity.hpp"
#include "tools/log.hpp"
#include <boost/algorithm/string/join.hpp>

using namespace Leosac;

RemoteControlSecurity::RemoteControlSecurity(const boost::property_tree::ptree &cfg)
    : cfg_(cfg)
    , unrestricted_(false)
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
    else
        unrestricted_ = true;
}

bool RemoteControlSecurity::allow_request(const std::string &user_pubkey,
                                          const std::string &req)
{
    if (unrestricted_)
        return true;

    if (default_permissions_.count(user_pubkey) == 0 ||
        permissions_.count(user_pubkey) == 0)
    {
        WARN("Received command from "
             << user_pubkey
             << " but no permission information for this user. Denying.");
        return false;
    }

    const auto &cmds = permissions_[user_pubkey];
    if (default_permissions_[user_pubkey]) // check cmd is not explicitly denied
        return std::find(cmds.begin(), cmds.end(), req) == cmds.end();
    else
        return std::find(cmds.begin(), cmds.end(), req) != cmds.end();
}

void RemoteControlSecurity::process_security_entry(
    const boost::property_tree::ptree &entry)
{
    const std::string &pk = entry.get<std::string>("pk");
    bool default_access   = entry.get<bool>("default", false);

    default_permissions_[pk] = default_access;
    // all child of entry, except "pk" and "default" are command name.
    for (const auto &c : entry)
    {
        if (c.first == "pk" || c.first == "default")
            continue;
        std::string cmd_name = c.first;
        bool allowed         = c.second.get_value<bool>();

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
