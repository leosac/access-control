#pragma once

#include <memory>
#include "IUser.hpp"
#include "core/auth/AuthTarget.hpp"

namespace Leosac
{
    namespace Auth
    {
        class IAuthenticationSource;

        using IAuthenticationSourcePtr = std::shared_ptr<IAuthenticationSource>;

        /**
        * An Auth source is a card id, a pin code, a fingerprint, etc...
        *
        * Those sources can be combined to provide strong authentication mechanism.
        * This class handle this.
        */
        class IAuthenticationSource
        {
        public:
            virtual ~IAuthenticationSource() = default;

            /**
            * Adds a new authentication sources as a subsource of this one.
            */
            virtual void addAuthSource(IAuthenticationSourcePtr source) = 0;

            /**
            * Set the user that map to this source,
            */
            virtual void owner(IUserPtr user) = 0;

            /**
            * Retrieve the user that map to this source.
            */
            virtual IUserPtr owner() const = 0;
        };
    }
}
