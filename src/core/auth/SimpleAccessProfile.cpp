#include <assert.h>
#include <tools/log.hpp>
#include "SimpleAccessProfile.hpp"
#include "AuthTarget.hpp"

using namespace Leosac::Auth;

bool SimpleAccessProfile::isAccessGranted(const std::chrono::system_clock::time_point &date, AuthTargetPtr target)
{
    if (!target || !time_frames_[target->name()].size())
    {
        // use default target information, as we have nothing specific
        for (const auto &time_frame : default_time_frames_)
            if (check_timeframe(time_frame, date))
                return true;
    }
    else
    {
        for (const auto &time_frame : time_frames_[target->name()])
            if (check_timeframe(time_frame, date))
                return true;
    }
    return false;
}

void SimpleAccessProfile::addAccessHour(AuthTargetPtr target,
        int day, int start_hour, int start_min, int end_hour, int end_min)
{
    assert(day >= 0 && day <= 7);
    assert(start_hour >= 0 && start_hour < 24);
    assert(end_hour >= 0 && end_hour < 24);
    assert(start_min >= 0 && start_min <= 60);
    assert(end_min >= 0 && end_min <= 60);

    SingleTimeFrame tf;

    tf.authorized = true;
    tf.day = day;
    tf.start_hour = start_hour;
    tf.start_min = start_min;
    tf.end_hour = end_hour;
    tf.end_min = end_min;

    if (!target)
        default_time_frames_.push_back(tf);
    else
        time_frames_[target->name()].push_back(tf);
}

bool SimpleAccessProfile::check_timeframe(const SingleTimeFrame &tf, const std::chrono::system_clock::time_point &date)
{
    std::time_t time_temp = std::chrono::system_clock::to_time_t(date);
    std::tm const *time_out = std::localtime(&time_temp);

    if (tf.day != time_out->tm_wday)
        return false;
    if (!(tf.start_hour < time_out->tm_hour ||
            (tf.start_hour == time_out->tm_hour && tf.start_min <= time_out->tm_min)))
        return false;
    if (!(tf.end_hour > time_out->tm_hour ||
            (tf.end_hour == time_out->tm_hour && tf.end_min >= time_out->tm_min)))
        return false;
    return true;
}
