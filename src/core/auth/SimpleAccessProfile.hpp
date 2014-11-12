#pragma once

#include <chrono>
#include <memory>
#include <core/auth/Interfaces/IAccessProfile.hpp>
#include <map>

namespace Leosac
{
    namespace Auth
    {
        struct TimeSlot
        {
            TimeSlot() : authorized(false),
                         start_hour(0),
                         start_min(0),
                         end_hour(0),
                         end_min(0)
            {}
            bool  authorized;
            int start_hour;
            int start_min;
            int end_hour;
            int end_min;
        };

        class SimpleAccessProfile;
        using SimpleAccessProfilePtr = std::shared_ptr<SimpleAccessProfile>;

        /**
        * Concrete implementation of a simple access control class.
        */
        class SimpleAccessProfile : public IAccessProfile
        {
        public:
            virtual bool isAccessGranted(const std::chrono::system_clock::time_point &date,
                    const std::string &target) override;


            /**
            * Add range in which access is allowed.
            * @warning Only one range per day is currently supported.
            */
            virtual void addAccessHour(int day, int start_hour, int start_min, int end_hour, int end_min);

        protected:
            std::map<int, TimeSlot> access_range_;
        };
    }
}
