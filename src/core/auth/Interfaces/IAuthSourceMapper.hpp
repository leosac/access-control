#pragma once

namespace Leosac
{
    namespace Auth
    {
        class IAuthenticationSource;
        class WiegandCard;

        /**
        * Visitor pattern to map AuthenticationSource to User.
        * Map can use this to help doing their mapping, for example a file
        * base authentication module would implement a FileAuthSourceMapper while
        * a database backed one would write DBAuthSourceMapper.
        */
        class IAuthSourceMapper
        {
        public:
            virtual ~IAuthSourceMapper() = default;

            virtual void visit(IAuthenticationSource *src) = 0;
            virtual void visit(WiegandCard *src) = 0;
        };

    }
}
