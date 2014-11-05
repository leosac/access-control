#pragma once

#include <string>
#include <core/auth/WiegandCard.hpp>
#include "core/auth/Interfaces/IAuthSourceMapper.hpp"

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

                virtual void visit(::Leosac::Auth::IAuthenticationSource *src);
                virtual void visit(::Leosac::Auth::WiegandCard *src);
            };
        }
    }
}
