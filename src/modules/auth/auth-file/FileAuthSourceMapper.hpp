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
#include "core/auth/SimpleAccessProfile.hpp"
#include "core/auth/Group.hpp"
#include "core/auth/WiegandCard.hpp"
#include "core/auth/Interfaces/IAuthSourceMapper.hpp"
#include "core/auth/Interfaces/IAuthenticationSource.hpp"

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
                * Build user/group permissions from configuration.
                */
                void build_permission();

                /**
                * We are building permissions for an user.
                */
                void permission_user(const std::string &user_name,
                        const boost::property_tree::ptree &);

                /**
                * We are building permissions for a group.
                */
                void permission_group(const std::string &group_name,
                        const boost::property_tree::ptree &);

                /**
                * Extract group membership.
                */
                void membership_group(const boost::property_tree::ptree &group_mapping);

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

                /**
                * Build the schedule for an access profile.
                *
                * It adds some schedule to an existing profile.
                * @param profile cannot be null
                * @param schedule_cfg property_tree that contains data from a `<schedule>` block.
                * @param is_default is this a `default_schedule` block ?
                */
                void build_schedule(Leosac::Auth::SimpleAccessProfilePtr profile,
                        const boost::property_tree::ptree &schedule_cfg, bool is_default);

                /**
                * Maps string day to int day (starting with sunday = 0)
                */
                static int week_day_to_int(const std::string &day);

                /**
                * Store the name of the configuration file.
                */
                std::string config_file_;

                /**
                * Property tree that holds the whole authentication tree.
                */
                boost::property_tree::ptree authentication_data_;

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
            };
        }
    }
}
