#pragma once

#include <memory>
#include "IAccessProfile.hpp"

namespace Leosac
{
    namespace Auth
    {
        class IAccessProfile;
        /**
        * Represent a user
        */
        class IUser
        {
        public:

        protected:
            std::unique_ptr<IAccessProfile> profile_;
        };
    }
}
