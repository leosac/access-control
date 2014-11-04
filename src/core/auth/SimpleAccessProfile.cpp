#include <assert.h>
#include "SimpleAccessProfile.hpp"

using namespace Leosac::Auth;

bool SimpleAccessProfile::isAccessGranted(const std::chrono::system_clock::time_point &date, const std::string &target)
{
    using namespace std::chrono;

    std::time_t time_temp = system_clock::to_time_t(date);
    std::tm const *time_out = std::localtime(&time_temp);

    TimeSlot &ts = access_range_[time_out->tm_wday];

    if (ts.authorized)
    {
        if (ts.start_hour <= time_out->tm_hour &&
                ts.start_min <= time_out->tm_min &&
                ts.end_hour >= time_out->tm_hour &&
                ts.end_min >= time_out->tm_min)
        {
            return true;
        }
    }
    return false;

}

void SimpleAccessProfile::addAccessHour(int day, int start_hour, int start_min, int end_hour, int end_min)
{
    assert(day >= 0 && day <= 7);
    assert(start_hour >= 0 && start_hour < 24);
    assert(end_hour >= 0 && end_hour < 24);
    assert(start_min >= 0 && start_min <= 60);
    assert(end_min >= 0 && end_min <= 60);

    access_range_[day].authorized = true;
    access_range_[day].start_hour = start_hour;
    access_range_[day].end_hour = end_hour;
    access_range_[day].start_min = start_min;
    access_range_[day].end_min = end_min;
}
