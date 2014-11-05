#pragma once

#include <string>
#include <boost/property_tree/ptree.hpp>
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

            private:

                /**
                * Store the name of the config.
                * This is only used in case we need to report an error.
                */
                std::string config_file_;

                /**
                * Property tree that holds the whole authentication tree.
                */
                boost::property_tree::ptree authentication_data_;
            };
        }
    }
}
