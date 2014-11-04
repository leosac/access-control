#include <gtest/gtest.h>
#include "core/auth/IAccessProfile.hpp"
#include "core/auth/IUser.hpp"
#include "core/auth/WiegandCard.hpp"

using namespace Leosac::Auth;

class AccessControlTest : public ::testing::Test
{
public:
    AccessControlTest() :
            user(new IUser("my_uuid"))
    {
        card = new WiegandCard("01:f2:fe:23", 32);
        card->owner(user);
    }

    ~AccessControlTest()
    {
        delete card;
    }

    IUserPtr user;
    IAccessProfile *profile;
    ICard *card;
};

TEST_F(AccessControlTest, Bla)
{
    ASSERT_EQ("my_uuid", card->owner()->id());
}
