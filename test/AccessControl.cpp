#include <gtest/gtest.h>
#include "core/auth/Interfaces/IUser.hpp"
#include "core/auth/WiegandCard.hpp"
#include "core/auth/SimpleAccessProfile.hpp"

using namespace Leosac::Auth;

class AccessControlTest : public ::testing::Test
{
public:
    AccessControlTest() :
            user(new IUser("my_uuid")),
            profile(new SimpleAccessProfile())
    {
        card = new WiegandCard("01:f2:fe:23", 32);
        card->owner(user);
        user->profile(profile);
    }

    ~AccessControlTest()
    {
        delete card;
    }

    IUserPtr user;
    IAccessProfilePtr profile;
    ICard *card;
};

TEST_F(AccessControlTest, Bla)
{
    ASSERT_EQ("my_uuid", card->owner()->id());
    ASSERT_FALSE(profile->isAccessGranted(std::chrono::system_clock::now(), "door_1"));
}
