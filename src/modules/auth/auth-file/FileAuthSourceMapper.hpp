#pragma once

#include <string>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <core/auth/SimpleAccessProfile.hpp>
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

                virtual void mapToUser(Leosac::Auth::IAuthenticationSourcePtr auth_source);

                virtual Leosac::Auth::IAccessProfilePtr buildProfile(Leosac::Auth::IAuthenticationSourcePtr auth_source) override;

            private:

                /**
                * Build user/group permissions from configuration.
                */
                void build_permission();

                /**
                * Build the schedule for an access profile.
                *
                * It adds some schedule to an existing profile.
                * @param profile cannot be null
                * @param schedule_cfg property_tree that contains data from a `<schedule>` block.
                */
                void build_schedule(Leosac::Auth::SimpleAccessProfilePtr profile,
                        const boost::property_tree::ptree &schedule_cfg);

                /**
                * Maps string day to int day (starting with sunday = 0)
                */
                static int week_day_to_int(const std::string &day);

                /**
                * Store the name of the configuration file.
                * This is only used in case we need to report an error.
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
            };
        }
    }
}
