/*
    Copyright (C) 2014-2016 Leosac

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

#include "core/auth/AuthFwd.hpp"
#include "core/auth/AuthFwd.hpp"
#include "core/auth/Interfaces/IAuthSourceMapper.hpp"
#include "core/auth/Interfaces/IAuthenticationSource.hpp"
#include "core/auth/SimpleAccessProfile.hpp"
#include "core/credentials/CredentialFwd.hpp"
#include "tools/ScheduleMapping.hpp"
#include "tools/SingleTimeFrame.hpp"
#include "tools/XmlNodeNameEnforcer.hpp"
#include "tools/XmlScheduleLoader.hpp"
#include <boost/property_tree/ptree.hpp>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>

namespace Leosac
{
namespace Module
{
namespace Auth
{
/**
* Use a file to map auth source (card, PIN, etc) to user.
*/
class FileAuthSourceMapper
    : public ::Leosac::Auth::IAuthSourceMapper,
      public ::Leosac::Tools::Visitor<::Leosac::Cred::RFIDCard>,
      public ::Leosac::Tools::Visitor<::Leosac::Cred::PinCode>,
      public ::Leosac::Tools::Visitor<::Leosac::Cred::RFIDCardPin>
{
  public:
    FileAuthSourceMapper(const std::string &auth_file);

    /**
    * Try to map a wiegand card_id to a user.
    */
    virtual void visit(::Leosac::Cred::RFIDCard &src) override;

    /**
    * Try to map a PIN code to a user.
    */
    virtual void visit(::Leosac::Cred::PinCode &src) override;

    /**
    * Try to map a card id + a PIN code to a user.
    */
    virtual void visit(::Leosac::Cred::RFIDCardPin &src) override;

    virtual void mapToUser(Leosac::Cred::ICredentialPtr auth_source);

    virtual Leosac::Auth::IAccessProfilePtr
    buildProfile(Leosac::Cred::ICredentialPtr cred);

    std::vector<Leosac::Auth::GroupPtr> groups() const override;

  private:
    /**
    * Lookup a credentials by ID.
    */
    Cred::ICredentialPtr find_cred_by_alias(const std::string &alias);

    /**
     * Build an access for a user.
     *
     * This simply check for mapping which are linked directly to
     * the given user.
     */
    Leosac::Auth::SimpleAccessProfilePtr build_user_profile(Leosac::Auth::UserPtr u);

    Leosac::Auth::SimpleAccessProfilePtr
    build_group_profile(Leosac::Auth::GroupPtr g);

    Leosac::Auth::SimpleAccessProfilePtr
    build_cred_profile(Leosac::Cred::ICredentialPtr c);

    /**
    * Store the credential to the id <-> credential map if the id is
    * non-empty.
    */
    void add_cred_to_id_map(Leosac::Cred::ICredentialPtr credential);

    /**
    * Load users from configuration tree, storing them
    * in the `users_` map.
    */
    void load_users(const boost::property_tree::ptree &users);

    /**
    * Load the schedules information from the config tree.
    * @param schedules The `<schedules>` subtree.
    */
    void load_schedules(const boost::property_tree::ptree &schedules);

    /**
    * Interpret the schedule mapping content of the config file.
    * This effectively build access profile for user.
    */
    void map_schedules(const boost::property_tree::ptree &schedules_mapping);

    /**
    * Extract group membership.
    */
    void load_groups(const boost::property_tree::ptree &group_mapping);

    /**
    * Eager loading of credentials to avoid walking through the
    * ptree whenever we have to grant/deny an access.
    */
    void load_credentials(const boost::property_tree::ptree &credentials);

    /**
    * Naive method that bruteforce groups to try to find
    * membership for an user.
    *
    * @param u a non-null pointer to user.
    * @return all group the user is a member of.
    */
    std::vector<Leosac::Auth::GroupPtr> get_user_groups(Leosac::Auth::UserPtr u);

    /**
    * Merge a bunch of profiles together and returns a new profile.
     *
     * If the resulting profile contains the schedule, this method
     * will return nullptr.
    */
    Leosac::Auth::IAccessProfilePtr
    merge_profiles(const std::vector<Leosac::Auth::IAccessProfilePtr> profiles);

    Leosac::Auth::ValidityInfo
    extract_credentials_validity(const boost::property_tree::ptree &node);

    /**
    * Store the name of the configuration file.
    */
    std::string config_file_;

    /**
    * Maps user id (or name) to object.
    */
    std::map<std::string, Leosac::Auth::UserPtr> users_;

    /**
    * Maps group name to object.
    */
    std::map<std::string, Leosac::Auth::GroupPtr> groups_;

    /**
    * Maps card_id to object.
    */
    std::unordered_map<std::string, Leosac::Cred::RFIDCardPtr> rfid_cards_;


    /**
    * Maps PIN code to object.
    */
    std::unordered_map<std::string, Leosac::Cred::PinCodePtr> pin_codes_;

    /**
    * Maps WiegandCard + PIN code to object.
    */
    std::map<std::pair<std::string, std::string>, Leosac::Cred::RFIDCardPinPtr>
        rfid_cards_pin;

    /**
    * Maps credentials ID (from XML) to object.
    * If id is empty, the cred wont end up is this list.
    */
    std::unordered_map<std::string, Leosac::Cred::ICredentialPtr> id_to_cred_;

    Tools::XmlScheduleLoader xml_schedules_;

    /**
     * List of mappings defined in the configuration file.
     */
    std::vector<Tools::ScheduleMappingPtr> mappings_;

    /**
     * We store doors object, but really we only use the name
     * property.
     * On door object is created for each mapping that specify a door.
     */
    std::vector<Leosac::Auth::DoorPtr> doors_;

    Tools::XmlNodeNameEnforcer xmlnne_;
};
using FileAuthSourceMapperPtr = std::shared_ptr<FileAuthSourceMapper>;
}
}
}
