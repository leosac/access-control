/*
    Copyright (C) 2014-2015 Islog

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

#include <string>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <core/auth/WiegandCardPin.hpp>
#include <unordered_map>
#include <utility>
#include <tools/XmlScheduleLoader.hpp>
#include "core/auth/SimpleAccessProfile.hpp"
#include "core/auth/Group.hpp"
#include "core/auth/WiegandCard.hpp"
#include "core/auth/Interfaces/IAuthSourceMapper.hpp"
#include "core/auth/Interfaces/IAuthenticationSource.hpp"
#include "tools/SingleTimeFrame.hpp"
#include "core/auth/SimpleAccessProfile.hpp"

namespace Leosac
{
    namespace Module
    {
        namespace Auth
        {
            /**
            * Use a file to map auth source (card, PIN, etc) to user.
            */
            class FileAuthSourceMapper : public ::Leosac::Auth::IAuthSourceMapper
            {
              public:
                FileAuthSourceMapper(const std::string &auth_file);

                /**
                * Try to map a wiegand card_id to a user.
                */
                virtual void visit(::Leosac::Auth::WiegandCard *src) override;

                /**
                * Try to map a PIN code to a user.
                */
                virtual void visit(::Leosac::Auth::PINCode *src) override;

                /**
                * Try to map a Wiegand card id + a PIN code to a user.
                */
                virtual void visit(::Leosac::Auth::WiegandCardPin *src) override;

                virtual void mapToUser(Leosac::Auth::IAuthenticationSourcePtr auth_source);

                virtual Leosac::Auth::IAccessProfilePtr buildProfile(Leosac::Auth::IAuthenticationSourcePtr auth_source) override;

                std::vector<Leosac::Auth::GroupPtr> groups() const override;
            private:

                /**
                 * Make sure the current tag name is what we expect, otherwise
                 * throw ConfigException.
                 */
                void enforce_xml_node_name(const std::string &expected,
                                           const std::string &current);

                /**
                * Lookup a credentials by ID.
                */
                Leosac::Auth::IAuthenticationSourcePtr find_cred_by_id(const std::string &id);

                /**
                * Store the credential to the id <-> credential map if the id is
                * non-empty.
                */
                void add_cred_to_id_map(Leosac::Auth::IAuthenticationSourcePtr cred);

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
                * This add a schedule to a profile.
                * This is used by map_schedules.
                */
                void add_schedule_to_profile(const std::string &schedule_name,
                        ::Leosac::Auth::SimpleAccessProfilePtr profile,
                        const std::string &door_name);

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
                std::vector<Leosac::Auth::GroupPtr> get_user_groups(Leosac::Auth::IUserPtr u);

                /**
                * Merge a bunch of profiles together and returns a new profile.
                */
                Leosac::Auth::IAccessProfilePtr merge_profiles(const std::vector<Leosac::Auth::IAccessProfilePtr> profiles);

                Leosac::Auth::CredentialValidity extract_credentials_validity(const boost::property_tree::ptree &node);

                /**
                * Store the name of the configuration file.
                */
                std::string config_file_;

                /**
                * Maps user id (or name) to object.
                */
                std::map<std::string, Leosac::Auth::IUserPtr> users_;

                /**
                * Maps group name to object.
                */
                std::map<std::string, Leosac::Auth::GroupPtr> groups_;

                /**
                * Maps target (eg door) name to object.
                */
                std::map<std::string, Leosac::Auth::AuthTargetPtr> targets_;

                /**
                * Maps card_id to object.
                */
                std::unordered_map<std::string, Leosac::Auth::WiegandCardPtr> wiegand_cards_;

                /**
                * Maps PIN code to object.
                */
                std::unordered_map<std::string, Leosac::Auth::PINCodePtr> pin_codes_;

                /**
                * Maps WiegandCard + PIN code to object.
                */
                std::map<std::pair<std::string, std::string>, Leosac::Auth::WiegandCardPinPtr> wiegand_cards_pins_;

                /**
                * Maps credentials ID to object.
                * If id is empty, the cred wont end up is this list.
                */
                std::unordered_map<std::string, Leosac::Auth::IAuthenticationSourcePtr> id_to_cred_;

                Tools::XmlScheduleLoader xml_schedules_;
            };
        using FileAuthSourceMapperPtr = std::shared_ptr<FileAuthSourceMapper>;
        }
    }
}
