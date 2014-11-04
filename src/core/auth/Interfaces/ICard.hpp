#pragma once

#include <memory>
#include "IUser.hpp"

namespace Leosac
{
    namespace Auth
    {
        class ICard;
        using ICardPtr = std::shared_ptr<ICard>;

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

            /**
            * Returns the Access Profile associated with this card.
            */
            virtual IAccessProfilePtr profile() const
            {

            }

        protected:
            /**
            * User that owns this card
            */
            IUserPtr owner_;
        };
    }
}
