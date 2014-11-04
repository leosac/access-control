#pragma once

#include <chrono>
#include <memory>

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
            * @param date the time point at which we try to get access
            * @param target the door we want to open
            */
            virtual bool isAccessGranted(const std::chrono::system_clock::time_point &date,
                    const std::string &target) = 0;


            /**
            * Add range in which access is allowed.
            * @warning Only one range per day is currently supported.
            */
            virtual void addAccessHour(int day, int start_hour, int start_min,
                    int end_hour, int end_min) = 0;
        };
    }
}
