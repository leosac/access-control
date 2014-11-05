#pragma once

#include "tools/IVisitor.hpp"
#include "core/auth/Interfaces/IAuthenticationSource.hpp"

namespace Leosac
{
    namespace Auth
    {
        /**
        * Base class to perform abstracted mapping operation over
        * various AuthSource object.
        *
        * It uses the visitor pattern to provide a somewhat low-coupling.
        * Each module are free to use their own implementation of IAuthSourceMapper
        * with various backend (database, file, ...). Theses shall be able to operate
        * on various AuthSource (card, pin code, fingerprint).
        */
        class IAuthSourceMapper : public Leosac::Tools::IVisitor
        {
        public:
            virtual ~IAuthSourceMapper() = default;

            /**
            * Must map the AuthenticationSource data to a User.
            *
            * This is done by setting the calling the `owner()` method on the auth source
            * object. In case it fails, the field shall be set to `nullptr`.
            *
            * @note The User may be `new`ed.
            */
            virtual void mapToUser(IAuthenticationSourcePtr auth_source) = 0;

            /**
            * Build an AccessProfile object given a AuthenticationSource.
            *
            * How the profile is built (ie, based only on user-id, based on auth source and
            * user-id, group membership, etc) is implementation-defined.
            *
            * The caller expect a valid profile object that will behave correctly.
            */
            virtual IAccessProfilePtr buildProfile(IAuthenticationSourcePtr auth_source) = 0;
        };

    }
}
