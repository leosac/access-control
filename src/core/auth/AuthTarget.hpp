#pragma once

#include <memory>
#include <string>

namespace Leosac
{
    namespace Auth
    {
        class AuthTarget;
        using AuthTargetPtr = std::shared_ptr<AuthTarget>;

        /**
        * Represent an object that we are authorizing against (a door).
        */
        class AuthTarget
        {
        public:
            virtual ~AuthTarget() = default;
            explicit AuthTarget(const std::string target_name);

            const std::string &name() const;
            void name(const std::string &new_name);

        protected:
            std::string name_;
        };
    }
}