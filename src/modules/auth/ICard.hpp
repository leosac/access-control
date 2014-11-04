#pragma once

namespace Leosac
{
    namespace Module
    {
        class IAccessProfile;
        class IUser;

        namespace Auth
        {
            /**
            * Represent a input card
            */
            class ICard
            {
            public:

            protected:
                /**
                * User that owns this card
                */
                IUser *owner_;
            };
        }
    }
}
