#pragma once

#include "tools/IVisitor.hpp"

namespace Leosac
{
    namespace Auth
    {
        /**
        * Visitor pattern to map AuthenticationSource to User.
        * Map can use this to help doing their mapping, for example a file
        * base authentication module would implement a FileAuthSourceMapper while
        * a database backed one would write DBAuthSourceMapper.
        */
        class IAuthSourceMapper : public Leosac::Tools::IVisitor
        {
        public:
            virtual ~IAuthSourceMapper() = default;

        };

    }
}
