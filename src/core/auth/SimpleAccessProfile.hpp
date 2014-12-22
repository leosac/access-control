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
            * @param target door we target. if this is null, it means this is a default access rule.
            */
            virtual void addAccessHour(AuthTargetPtr target,
                    int day, int start_hour, int start_min, int end_hour, int end_min);

            /**
            * Adds a time frame for a target.
            */
            virtual void addAccessTimeFrame(AuthTargetPtr target, const SingleTimeFrame& tf);

            /**
            * Provide informations about time frames that apply to non-specific door (ie when target is nullptr).
            */
            virtual const std::vector<SingleTimeFrame>& defaultTimeFrames() const;

            /**
            * Provide informations about the doors specific timeframe.
            */
            virtual const std::map<std::string, std::vector<SingleTimeFrame>>& timeFrames() const;

        protected:
            /**
            * Check whether this time frame permits access or not.
            *
            * @return true if the time frame grant access; false otherwise
            */
            bool check_timeframe(const SingleTimeFrame &tf, const std::chrono::system_clock::time_point &date);

            /**
            * Map target name to target's time frame.
            */
            std::map<std::string, std::vector<SingleTimeFrame>> time_frames_;

            /**
            * Time frames to use when we have nothing for a specific target.
            */
            std::vector<SingleTimeFrame> default_time_frames_;
        };
    }
}
