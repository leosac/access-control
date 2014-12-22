#pragma once

#include <memory>
#include "IAccessProfile.hpp"

namespace Leosac
{
    namespace Auth
    {
        /**
        * Represent a temporary input card.
        *
        * Temporary card still have an owner, but the owner is used
        * for reporting, not for permissions checking.
        *
        * Temporary card directly maps to an AccessProfile.
        */
        class TemporaryCard : public ICard
        {
        public:

        protected:
            IAccessProfilePtr profile_;
        };
    }
}
