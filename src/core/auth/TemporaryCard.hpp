#pragma once

#include <memory>

namespace Leosac
{
    class IAccessProfile;

    class IUser;

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
            std::unique_ptr<IAccessProfile> profile_;
        };
    }
}
