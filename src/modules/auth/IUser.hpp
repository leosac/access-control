#pragma once

namespace Leosac
{
    namespace Module
    {
        class IAccessProfile;
        namespace Auth
        {
            /**
            * Represent a user
            */
            class IUser
            {
            public:

            protected:
                IAccessProfile *profile_;
            };
        }
    }
}
