#pragma once

#include <chrono>
#include <memory>
#include <map>
#include <vector>
#include "core/auth/Interfaces/IAccessProfile.hpp"
#include "core/auth/AuthTarget.hpp"

namespace Leosac
{
    namespace Auth
    {
        /**
        * A single time frame, in a day.
        */
        struct SingleTimeFrame
        {
            SingleTimeFrame() : authorized(false),
                                day(0),
                                start_hour(0),
                                start_min(0),
                                end_hour(0),
                                end_min(0)
            {}
            bool authorized;
            int day;
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
                     AuthTargetPtr target) override;

            /**
            * Add range in which access is allowed.
            */
            virtual void addAccessHour(AuthTargetPtr target,
                    int day, int start_hour, int start_min, int end_hour, int end_min);

        protected:
            /**
            * Map target name to target's time frame.
            */
            std::map<std::string, std::vector<SingleTimeFrame>> time_frames_;
        };
    }
}
