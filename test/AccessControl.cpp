#include <gtest/gtest.h>
#include "core/auth/Interfaces/IUser.hpp"
#include "core/auth/WiegandCard.hpp"
#include "core/auth/SimpleAccessProfile.hpp"
#include "core/auth/AuthTarget.hpp"

using namespace Leosac::Auth;

class AccessControlTest : public ::testing::Test
{
public:
    AccessControlTest() :
            user(new IUser("my_uuid")),
            profile(new SimpleAccessProfile()),
            auth_source_(new BaseAuthSource()),
            card(new WiegandCard("01:f2:fe:23", 32)),
            target(new AuthTarget())

    {
        // this is authentication module work
        card->owner(user);
        auth_source_->addAuthSource(card);
        user->profile(profile);

        target->name("MY_DOOR");
    }

    ~AccessControlTest()
    {
    }

    IAuthenticationSourcePtr auth_source_;
    IUserPtr user;
    IAccessProfilePtr profile;
    WiegandCardPtr card;
    AuthTargetPtr target;
};

TEST_F(AccessControlTest, Bla)
{
//    ASSERT_EQ("my_uuid", card->owner()->id());
//    ASSERT_FALSE(profile->isAccessGranted(std::chrono::system_clock::now(), "door_1"));
}
