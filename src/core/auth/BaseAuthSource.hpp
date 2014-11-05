#pragma once

#include <memory>
#include <vector>
#include "core/auth/Interfaces/IAuthenticationSource.hpp"

namespace Leosac
{
    namespace Auth
    {
        class BaseAuthSource : public IAuthenticationSource
        {
        public:
            virtual ~BaseAuthSource() = default;
            virtual void addAuthSource(IAuthenticationSourcePtr source) override;

            virtual void accept(Tools::IVisitor *visitor) override;

            virtual void owner(IUserPtr user) override;

            virtual IUserPtr owner() const override;

            /**
            * This implementation simply return the owner_ profile, or null
            * if not available.
            */
            virtual IAccessProfilePtr profile() const;

        protected:
            /**
            * Which user this auth source maps to. May be null
            */
            IUserPtr owner_;

            /**
            * Underlying auth source.
            */
            std::vector<IAuthenticationSourcePtr> subsources_;
        };
    }
}
