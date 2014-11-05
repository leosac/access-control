#pragma once

#include <memory>
#include "IUser.hpp"
#include "core/auth/AuthTarget.hpp"
#include "tools/IVisitable.hpp"

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
        class IAuthenticationSource : public Tools::IVisitable
        {
        public:
            virtual ~IAuthenticationSource() = default;

            virtual void accept(Tools::IVisitor *visitor) override = 0;

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

            /**
            * Return (or generate) an Access Profile that make sense
            * based on the data available in this AuthenticationSource object.
            *
            * This profile will later be used to chose between granting and denying
            * access to an end user.
            */
            virtual IAccessProfilePtr profile() const = 0;
        };
    }
}
