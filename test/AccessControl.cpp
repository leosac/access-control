#include <gtest/gtest.h>
#include <chrono>
#include "core/auth/Interfaces/IUser.hpp"
#include "core/auth/WiegandCard.hpp"
#include "core/auth/SimpleAccessProfile.hpp"
#include "core/auth/AuthTarget.hpp"

using namespace Leosac::Auth;
namespace Leosac
{
    namespace Test
    {
        class AccessControlTest : public ::testing::Test
        {
        public:
            AccessControlTest() :
                    user(new IUser("my_uuid")),
                    profile(new SimpleAccessProfile()),
                    auth_source_(new BaseAuthSource()),
                    card(new WiegandCard("01:f2:fe:23", 32)),
                    target(new AuthTarget("random_target"))
            {
                // this is authentication module work
                card->owner(user);
                auth_source_->addAuthSource(card);
                auth_source_->owner(user);
                user->profile(profile);

                // add access monday from 10 to 14.
                auto profile_r = std::dynamic_pointer_cast<SimpleAccessProfile>(profile);
                // todo target
                profile_r->addAccessHour(nullptr, 1, 10, 0, 14, 0);

                target->name("MY_DOOR");
            }

            ~AccessControlTest()
            {
            }

            IUserPtr user;
            IAccessProfilePtr profile;
            IAuthenticationSourcePtr auth_source_;
            WiegandCardPtr card;
            AuthTargetPtr target;
        };

        TEST_F(AccessControlTest, Bla)
        {
            IAccessProfilePtr p;

            p = auth_source_->profile();

            // Monday
            std::tm date = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            date.tm_hour = 12;
            date.tm_mday = 3;
            date.tm_mon = 10;
            date.tm_year = 114;
            std::time_t time_temp = std::mktime(&date);
            std::tm const *time_out = std::localtime(&time_temp);
            ASSERT_EQ(1, time_out->tm_wday);

            // todo target
            ASSERT_TRUE(p->isAccessGranted(std::chrono::system_clock::from_time_t(time_temp), nullptr));

            std::tm cpy = *time_out;
            cpy.tm_hour = 14;
            cpy.tm_min = 20;
            std::time_t time_temp2 = std::mktime(&cpy);
            // todo target
            ASSERT_FALSE(p->isAccessGranted(std::chrono::system_clock::from_time_t(time_temp2), nullptr));

        }
    }
}
