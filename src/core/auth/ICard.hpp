#pragma once

#include <memory>
#include "IUser.hpp"

namespace Leosac
{
    namespace Auth
    {
        class IAccessProfile;
        class IUser;

        /**
        * Represent a input card
        */
        class ICard
        {
        public:
            virtual ~ICard() = default;

        protected:
            /**
            * User that owns this card
            */
            std::unique_ptr<IUser> owner_;
        };
    }
}
