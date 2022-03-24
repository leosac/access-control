/*
    Copyright (C) 2014-2022 Leosac

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

#pragma once

#include <boost/property_tree/ptree.hpp>
#include <unordered_map>
#include <vector>

namespace Leosac
{

class Kernel;

/**
* Provide some kind of security framework to the Remote Control service.
*
* This allows relatively fine-grained permissions and allows user
* to chose who has access to the remote control, and how much permission they got.
*
* Remote user are identified using the curve private key they use to connect.
*
* Implementation Notes:
*     We store a map of z85-encoded-pubkey <--> list (command name);
*     The meaning of this map depends on the default permission for user.
*
*     If the user is by default granted permission, then the map holds a list command
*     the user isn't allowed to perform. Otherwise, we store a list of allowed
* command.
*
*/
class RemoteControlSecurity
{
  public:
    /**
    * The config tree is the same tree that the RemoteControl object has.
    */
    RemoteControlSecurity(const boost::property_tree::ptree &cfg);

    bool allow_request(const std::string &user_pubkey, const std::string &req);

    /**
    * Map user public key to a set of command
    */
    using KeyCommandsMap = std::unordered_map<std::string, std::vector<std::string>>;

  private:
    void process_config();

    /**
    * Process one `<map>` entry.
    */
    void process_security_entry(const boost::property_tree::ptree &);

    boost::property_tree::ptree cfg_;

    std::unordered_map<std::string, bool> default_permissions_;
    KeyCommandsMap permissions_;

    /**
    * If everyone has access to everything.
    */
    bool unrestricted_;
};
}
