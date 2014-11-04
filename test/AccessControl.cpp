#include <gtest/gtest.h>
#include "core/auth/IAccessProfile.hpp"
#include "core/auth/IUser.hpp"
#include "core/auth/WiegandCard.hpp"

using namespace Leosac::Auth;

class AccessControlTest : public ::testing::Test
{
public:
    AccessControlTest()
    {
        card = new WiegandCard("01:f2:fe:23", 32);
    }

    ~AccessControlTest()
    {
        delete card;
        //delete profile;
    }

    IAccessProfile *profile;
    ICard *card;
};

TEST_F(AccessControlTest, Bla)
{

}
