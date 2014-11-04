#include "SimpleAccessProfile.hpp"

using namespace Leosac::Auth;

bool SimpleAccessProfile::isAccessGranted(const std::chrono::system_clock::time_point &date, const std::string &target)
{
    return false;
}
