#include <assert.h>
#include <tools/log.hpp>
#include "SimpleAccessProfile.hpp"
#include "AuthTarget.hpp"

using namespace Leosac::Auth;

bool SimpleAccessProfile::isAccessGranted(const std::chrono::system_clock::time_point &date, AuthTargetPtr target)
{
    using namespace std::chrono;
    assert(target);

    std::time_t time_temp = system_clock::to_time_t(date);
    std::tm const *time_out = std::localtime(&time_temp);
    for (const auto &time_frame : time_frames_[target->name()])
    {
        if (time_frame.day == time_out->tm_wday &&
                time_frame.start_hour <= time_out->tm_hour &&
                time_frame.start_min <= time_out->tm_min &&
                time_frame.end_hour >= time_out->tm_hour &&
                time_frame.end_min >= time_out->tm_min)
        {
            return true;
        }
    }
    return false;
}

void SimpleAccessProfile::addAccessHour(AuthTargetPtr target,
        int day, int start_hour, int start_min, int end_hour, int end_min)
{
    assert(target);
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

    time_frames_[target->name()].push_back(tf);
}
