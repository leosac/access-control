#pragma once

#include <chrono>
#include <memory>
#include "core/auth/AuthTarget.hpp"

namespace Leosac
{
    namespace Auth
    {
        class IAccessProfile;
        using IAccessProfilePtr = std::shared_ptr<IAccessProfile>;

        /**
        * Holds information about access permission
        */
        class IAccessProfile
        {
        public:
            /**
            * Does the profile allow access to the user.
            *
            * @param date the time point at which we try to get access.
            * @param target the door we want to open.
            *
            * @note Check for defaults permissions then door-specific permissions.
            */
            virtual bool isAccessGranted(const std::chrono::system_clock::time_point &date,
            AuthTargetPtr target) = 0;
        };
    }
}
