#pragma once

#include <memory>
#include <vector>
#include "core/auth/Interfaces/IAuthenticationSource.hpp"

namespace Leosac
{
    namespace Auth
    {
        class BaseAuthSource;
        using BaseAuthSourcePtr = std::shared_ptr<BaseAuthSource>;

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
            virtual IAccessProfilePtr profile() const override;

            virtual const std::string &name() const override;

            /**
            * Set a name for this auth source
            */
            void name(const std::string &n);

        protected:
            /**
            * Which user this auth source maps to. May be null
            */
            IUserPtr owner_;

            /**
            * Underlying auth source.
            */
            std::vector<IAuthenticationSourcePtr> subsources_;

            /**
            * Name of the source (generally the module / device that sent it)
            */
            std::string source_name_;
        };
    }
}
