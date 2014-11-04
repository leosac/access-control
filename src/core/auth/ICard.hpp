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

            virtual IUserPtr owner() const noexcept
            {
                return owner_;
            }

            virtual void owner(IUserPtr user)
            {
                owner_ = user;
            }


        protected:
            /**
            * User that owns this card
            */
            IUserPtr owner_;
        };
    }
}
