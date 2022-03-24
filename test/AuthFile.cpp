/*
    Copyright (C) 2014-2022 Leosac

    This file is part of Leosac.

    Leosac is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leosac is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/**
* @file
* This file contains unit test suite related to the AuthFile module.
*
* @author xaqq
*/

#include "core/auth/Interfaces/IAuthSourceMapper.hpp"
#include "core/auth/User.hpp"
#include "core/credentials/PinCode.hpp"
#include "core/credentials/RFIDCard.hpp"
#include "core/credentials/RFIDCardPin.hpp"
#include "modules/auth/auth-file/FileAuthSourceMapper.hpp"
#include "tools/unixshellscript.hpp"
#include <chrono>
#include <exception/moduleexception.hpp>
#include <gtest/gtest.h>

/**
* Path to test-data file.
* this come from command line (see CMakeLists.txt)
*/
static std::string gl_data_path;

using namespace Leosac::Auth;
using namespace Leosac::Module::Auth;

namespace Leosac
{
namespace Test
{
/**
* Test the mapping of wiegand-card to user from a file.
*
* @note This test suite use the AuthFile-*.xml files.
*/
class AuthFileMapperTest : public ::testing::Test
{
  public:
    AuthFileMapperTest()
        : doorA_(new AuthTarget("doorA"))
        , doorB_(new AuthTarget("doorB"))
        , doorC_(new AuthTarget("doorC"))
        , my_card_(new Cred::RFIDCard())
        , my_card2_(new Cred::RFIDCard())
        , unknown_card_(new Cred::RFIDCard())
        , my_pin_(new Cred::PinCode())
    {
        my_card_->card_id("aa:bb:cc:dd");
        my_card_->nb_bits(32);

        my_card2_->card_id("cc:dd:ee:ff");
        my_card2_->nb_bits(32);

        unknown_card_->card_id("00:00:00:00");
        unknown_card_->nb_bits(32);

        my_pin_->pin_code("1234");

        card_and_pin_ = std::make_shared<Cred::RFIDCardPin>();
        card_and_pin_->card().card_id("cc:dd:ee:ff");
        card_and_pin_->card().nb_bits(32);
        card_and_pin_->pin().pin_code("1234");

        mapper_  = new FileAuthSourceMapper(gl_data_path + "AuthFile-1.xml");
        mapper2_ = new FileAuthSourceMapper(gl_data_path + "AuthFile-3.xml");
        mapper3_ = new FileAuthSourceMapper(gl_data_path + "AuthFile-4.xml");
        mapper4_ = new FileAuthSourceMapper(gl_data_path + "AuthFile-5.xml");
        mapper5_ = new FileAuthSourceMapper(gl_data_path + "AuthFile-6.xml");
        mapper6_ = new FileAuthSourceMapper(gl_data_path + "AuthFile-7.xml");
        mapper7_ = new FileAuthSourceMapper(gl_data_path + "AuthFile-8.xml");

        // initialize date object.
        std::tm date = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        std::tm date2;
        std::tm date3;
        std::tm date4;
        std::tm date5;
        std::time_t time_temp;

        // Monday 12h
        date.tm_year = 114;
        date.tm_mon  = 10;
        date.tm_mday = 3;
        date.tm_hour = 12;
        date5 = date4 = date3 = date2 = date;

        time_temp         = std::mktime(&date);
        date_monday_12_00 = std::chrono::system_clock::from_time_t(time_temp);

        // monday 16h31
        date2.tm_hour     = 16;
        date2.tm_min      = 31;
        time_temp         = std::mktime(&date2);
        date_monday_16_31 = std::chrono::system_clock::from_time_t(time_temp);

        // sunday 18h50
        date3.tm_mday     = 2;
        date3.tm_hour     = 18;
        date3.tm_min      = 50;
        time_temp         = std::mktime(&date3);
        date_sunday_18_50 = std::chrono::system_clock::from_time_t(time_temp);

        // wednesday 23h42
        date4.tm_mday        = 5;
        date4.tm_hour        = 23;
        date4.tm_min         = 42;
        time_temp            = std::mktime(&date4);
        date_wednesday_23_42 = std::chrono::system_clock::from_time_t(time_temp);

        // thursday 14h00
        date4.tm_mday       = 6;
        date4.tm_hour       = 14;
        date4.tm_min        = 00;
        time_temp           = std::mktime(&date4);
        date_thursday_14_00 = std::chrono::system_clock::from_time_t(time_temp);
    }

    ~AuthFileMapperTest()
    {
        delete mapper_;
        delete mapper2_;
        delete mapper3_;
        delete mapper4_;
        delete mapper5_;
        delete mapper6_;
        delete mapper7_;
    }

    bool is_in_group(const std::string &user_name, const std::string &group_name,
                     IAuthSourceMapper *mapper)
    {
        auto search_lambda = [&](UserPtr u) -> bool {
            return u->username() == boost::algorithm::to_lower_copy(user_name);
        };

        for (const auto &group : mapper->groups())
        {
            if (group->name() == group_name)
            {
                return std::find_if(group->members().begin(), group->members().end(),
                                    search_lambda) != group->members().end();
            }
        }
        return false;
    }

    std::chrono::system_clock::time_point date_monday_12_00;
    std::chrono::system_clock::time_point date_monday_16_31;
    std::chrono::system_clock::time_point date_sunday_18_50;
    std::chrono::system_clock::time_point date_wednesday_23_42;
    std::chrono::system_clock::time_point date_thursday_14_00;
    AuthTargetPtr doorA_;
    AuthTargetPtr doorB_;
    AuthTargetPtr doorC_;
    IAuthSourceMapper *mapper_;
    // for group related tests.
    IAuthSourceMapper *mapper2_;
    IAuthSourceMapper *mapper3_;
    // groupe + single user permissions
    IAuthSourceMapper *mapper4_;
    // wiegand card + pin
    IAuthSourceMapper *mapper5_;
    // cred validity
    IAuthSourceMapper *mapper6_;
    // per-credentials schedules.
    IAuthSourceMapper *mapper7_;
    Cred::RFIDCardPtr my_card_;
    Cred::RFIDCardPtr my_card2_;
    Cred::RFIDCardPtr unknown_card_;
    Cred::PinCodePtr my_pin_;
    Cred::RFIDCardPinPtr card_and_pin_;
};

/**
* Successful mapping
*/
TEST_F(AuthFileMapperTest, SimpleMapping)
{
    ASSERT_FALSE(my_card_->owner().get());
    mapper_->mapToUser(my_card_);
    ASSERT_TRUE(my_card_->owner().get());
    ASSERT_EQ("my_user", my_card_->owner()->username());

    // MY_USER with a PIN Code.
    ASSERT_FALSE(my_pin_->owner().get());
    mapper_->mapToUser(my_pin_);
    ASSERT_TRUE(my_pin_->owner().get());
    ASSERT_EQ("my_user", my_pin_->owner()->username());

    ASSERT_FALSE(my_card2_->owner().get());
    mapper_->mapToUser(my_card2_);
    ASSERT_TRUE(my_card2_->owner().get());
    ASSERT_EQ("toto", my_card2_->owner()->username());
}

/**
* Test that time frame for access a properly loaded and that
* the AccessProfile is coherent with the configuration file.
*/
TEST_F(AuthFileMapperTest, TimeFrameMapping)
{
    mapper_->mapToUser(my_card_);
    ASSERT_TRUE(my_card_->owner().get());
    IAccessProfilePtr profile = mapper_->buildProfile(my_card_);
    // with wiegand card
    ASSERT_TRUE(profile.get());

    ASSERT_TRUE(profile->isAccessGranted(date_monday_12_00, doorA_));
    ASSERT_TRUE(profile->isAccessGranted(date_monday_16_31, doorA_));
    ASSERT_FALSE(profile->isAccessGranted(date_thursday_14_00, doorA_));

    ASSERT_TRUE(profile->isAccessGranted(date_monday_12_00, doorB_));
    ASSERT_TRUE(profile->isAccessGranted(date_monday_16_31, doorB_));
    ASSERT_FALSE(profile->isAccessGranted(date_thursday_14_00, doorB_));

    ASSERT_TRUE(profile->isAccessGranted(date_sunday_18_50, doorA_));
    ASSERT_TRUE(profile->isAccessGranted(date_sunday_18_50, doorB_));

    // same profile. but using a PIN code instead.
    mapper_->mapToUser(my_pin_);
    ASSERT_TRUE(my_pin_->owner().get());
    IAccessProfilePtr profile_from_pin = mapper_->buildProfile(my_pin_);
    // with pin code.
    ASSERT_TRUE(profile_from_pin.get());

    ASSERT_TRUE(profile_from_pin->isAccessGranted(date_monday_12_00, doorA_));
    ASSERT_TRUE(profile_from_pin->isAccessGranted(date_monday_16_31, doorA_));
    ASSERT_FALSE(profile_from_pin->isAccessGranted(date_thursday_14_00, doorA_));

    ASSERT_TRUE(profile_from_pin->isAccessGranted(date_monday_12_00, doorB_));
    ASSERT_TRUE(profile_from_pin->isAccessGranted(date_monday_16_31, doorB_));
    ASSERT_FALSE(profile_from_pin->isAccessGranted(date_thursday_14_00, doorB_));

    ASSERT_TRUE(profile_from_pin->isAccessGranted(date_sunday_18_50, doorA_));
    ASSERT_TRUE(profile_from_pin->isAccessGranted(date_sunday_18_50, doorB_));
}

/**
* Test time frame with default_schedule param
*/
TEST_F(AuthFileMapperTest, TimeFrameMapping2)
{
    mapper_->mapToUser(my_card_);
    mapper_->mapToUser(my_card2_);
    ASSERT_TRUE(my_card_->owner().get());
    ASSERT_TRUE(my_card2_->owner().get());
    IAccessProfilePtr profile_myuser = mapper_->buildProfile(my_card_);
    IAccessProfilePtr profile_toto   = mapper_->buildProfile(my_card2_);
    ASSERT_TRUE(profile_myuser.get());
    ASSERT_TRUE(profile_toto.get());


    ASSERT_TRUE(profile_myuser->isAccessGranted(date_sunday_18_50, doorC_));
    ASSERT_TRUE(profile_myuser->isAccessGranted(date_monday_16_31, doorC_));

    ASSERT_TRUE(profile_toto->isAccessGranted(date_sunday_18_50, doorA_));
    ASSERT_TRUE(profile_toto->isAccessGranted(date_sunday_18_50, doorB_));
    ASSERT_TRUE(profile_toto->isAccessGranted(date_sunday_18_50, doorC_));
    ASSERT_FALSE(profile_toto->isAccessGranted(date_monday_16_31, doorC_));
}

/**
* Card ID doesn't exist in the file.
*/
TEST_F(AuthFileMapperTest, NotFoundMapping)
{
    ASSERT_FALSE(unknown_card_->owner().get());
    mapper_->mapToUser(unknown_card_);
    ASSERT_FALSE(unknown_card_->owner().get());
}

/**
* File is not accessible
*/
TEST_F(AuthFileMapperTest, InvalidFile)
{
    ASSERT_THROW(std::unique_ptr<IAuthSourceMapper> faulty_mapper(
                     new FileAuthSourceMapper(gl_data_path + "no_file")),
                 ModuleException);
}

/**
* AuthFile-2.xml has invalid content.
*/
TEST_F(AuthFileMapperTest, InvalidFileContent)
{
    ASSERT_THROW(
        {
            std::unique_ptr<IAuthSourceMapper> faulty_mapper(
                new FileAuthSourceMapper(gl_data_path + "AuthFile-2.xml"));
            faulty_mapper->mapToUser(my_card_);
        },
        ModuleException);
}

TEST_F(AuthFileMapperTest, TestGroupMapping)
{
    ASSERT_TRUE(is_in_group("my_user", "Admins", mapper2_));
    ASSERT_TRUE(is_in_group("toto", "Admins", mapper2_));

    ASSERT_FALSE(is_in_group("useless", "Admins", mapper2_));
    ASSERT_TRUE(is_in_group("useless", "random_group", mapper2_));
}

TEST_F(AuthFileMapperTest, TestMultiGroupMapping)
{
    // MY_USER has 4 two group here.
    ASSERT_TRUE(is_in_group("my_user", "Admins", mapper3_));
    ASSERT_TRUE(is_in_group("my_user", "Operators", mapper3_));
    ASSERT_TRUE(is_in_group("my_user", "Users", mapper3_));
    ASSERT_TRUE(is_in_group("my_user", "YetAnotherGroup", mapper3_));
    ASSERT_FALSE(is_in_group("my_user", "group_doesnt_exists", mapper3_));

    ASSERT_FALSE(is_in_group("toto", "Operators", mapper3_));
    ASSERT_TRUE(is_in_group("toto", "Admins", mapper3_));
    ASSERT_TRUE(is_in_group("toto", "Users", mapper3_));
}

/**
* Test that group permission applies to user.
*/
TEST_F(AuthFileMapperTest, TestGroupPermission)
{
    mapper2_->mapToUser(my_card_);

    auto profile = mapper2_->buildProfile(my_card_);
    ASSERT_TRUE(profile.get());
    ASSERT_TRUE(is_in_group("my_user", "Admins", mapper2_));

    ASSERT_TRUE(profile->isAccessGranted(date_monday_12_00, doorA_));
    ASSERT_FALSE(profile->isAccessGranted(date_monday_16_31, doorA_));

    ASSERT_FALSE(profile->isAccessGranted(date_monday_12_00, doorB_));
    ASSERT_TRUE(profile->isAccessGranted(date_monday_16_31, doorB_));

    ASSERT_TRUE(profile->isAccessGranted(date_sunday_18_50, doorA_));
    ASSERT_TRUE(profile->isAccessGranted(date_sunday_18_50, doorB_));

    ASSERT_TRUE(profile->isAccessGranted(date_sunday_18_50, doorC_));
    ASSERT_FALSE(profile->isAccessGranted(date_monday_16_31, doorC_));
}

/**
* Tests that permissions from multiple groups are added together.
* If a user is in 2 groups it should have both group permissions.
*/
TEST_F(AuthFileMapperTest, TestMultiGroupPermission)
{
    mapper3_->mapToUser(my_card_);
    mapper3_->mapToUser(my_card2_);

    auto profile      = mapper3_->buildProfile(my_card_);
    auto profile_toto = mapper3_->buildProfile(my_card2_);
    ASSERT_TRUE(profile.get());
    ASSERT_TRUE(profile_toto.get());

    ASSERT_TRUE(profile->isAccessGranted(date_monday_12_00, doorA_));
    ASSERT_FALSE(profile->isAccessGranted(date_monday_16_31, doorA_));

    ASSERT_FALSE(profile->isAccessGranted(date_monday_12_00, doorB_));
    ASSERT_TRUE(profile->isAccessGranted(date_monday_16_31, doorB_));

    ASSERT_TRUE(profile->isAccessGranted(date_sunday_18_50, doorA_));
    ASSERT_TRUE(profile->isAccessGranted(date_sunday_18_50, doorB_));

    ASSERT_TRUE(profile->isAccessGranted(date_sunday_18_50, doorC_));
    ASSERT_TRUE(profile->isAccessGranted(date_monday_16_31, doorC_));

    // full wednesday access because of group user
    ASSERT_TRUE(profile->isAccessGranted(date_wednesday_23_42, doorA_));
    ASSERT_TRUE(profile->isAccessGranted(date_wednesday_23_42, doorB_));
    ASSERT_TRUE(profile->isAccessGranted(date_wednesday_23_42, doorC_));

    // doorA and B access thursday because of group YetAnotherGroup
    ASSERT_TRUE(profile->isAccessGranted(date_thursday_14_00, doorA_));
    ASSERT_TRUE(profile->isAccessGranted(date_thursday_14_00, doorB_));
    ASSERT_FALSE(profile->isAccessGranted(date_thursday_14_00, doorC_));

    // Toto is in users, but not YetAnotherGroup. Should only have access to doorA
    ASSERT_TRUE(profile_toto->isAccessGranted(date_thursday_14_00, doorA_));
    ASSERT_FALSE(profile_toto->isAccessGranted(date_thursday_14_00, doorB_));
    ASSERT_FALSE(profile_toto->isAccessGranted(date_thursday_14_00, doorC_));
}

/**
* Tests that single-user permission and group permission works
* well together.
*/
TEST_F(AuthFileMapperTest, TestGroupAndUserPermission)
{
    mapper4_->mapToUser(my_card_);
    mapper4_->mapToUser(my_card2_);

    auto profile      = mapper4_->buildProfile(my_card_);
    auto profile_toto = mapper4_->buildProfile(my_card2_);
    ASSERT_TRUE(profile.get());
    ASSERT_TRUE(profile_toto.get());

    ASSERT_TRUE(profile->isAccessGranted(date_sunday_18_50, doorB_));
    ASSERT_TRUE(profile_toto->isAccessGranted(date_sunday_18_50, doorB_));

    ASSERT_TRUE(profile->isAccessGranted(date_thursday_14_00, doorA_));
    ASSERT_TRUE(profile_toto->isAccessGranted(date_thursday_14_00, doorA_));

    ASSERT_TRUE(profile->isAccessGranted(date_sunday_18_50, doorA_));
    ASSERT_FALSE(profile_toto->isAccessGranted(date_sunday_18_50, doorA_));
}

TEST_F(AuthFileMapperTest, UnkownCardId)
{
    mapper4_->mapToUser(unknown_card_);
    auto profile = mapper4_->buildProfile(unknown_card_);
    ASSERT_FALSE(profile.get());
}

TEST_F(AuthFileMapperTest, TestWiegandCardAndPin)
{
    mapper5_->mapToUser(card_and_pin_);
    auto profile_toto = mapper5_->buildProfile(card_and_pin_);
    ASSERT_TRUE(profile_toto.get());

    ASSERT_TRUE(profile_toto->isAccessGranted(date_sunday_18_50, doorA_));
    ASSERT_TRUE(profile_toto->isAccessGranted(date_sunday_18_50, doorB_));
    ASSERT_TRUE(profile_toto->isAccessGranted(date_sunday_18_50, doorC_));
    ASSERT_FALSE(profile_toto->isAccessGranted(date_monday_16_31, doorC_));
}

TEST_F(AuthFileMapperTest, TestUserValidityLimit)
{
    mapper5_->mapToUser(my_card_);
    mapper5_->mapToUser(my_card2_);
    mapper5_->mapToUser(my_pin_);
    // this user is disabled. should have a NULL profile.
    auto profile_llamasticot = mapper5_->buildProfile(my_card_);
    ASSERT_FALSE(profile_llamasticot.get());

    auto profile_llama1 = mapper5_->buildProfile(my_card2_);
    ASSERT_TRUE(profile_llama1.get());

    auto profile_llama2 = mapper5_->buildProfile(my_pin_);
    ASSERT_FALSE(profile_llama2.get());
}

TEST_F(AuthFileMapperTest, TestCredentialsValidityLimit)
{
    mapper6_->mapToUser(my_card_);
    mapper6_->mapToUser(my_card2_);
    mapper6_->mapToUser(my_pin_);
    // this user is disabled. shouldn't have a profile.
    auto profile_llamasticot = mapper6_->buildProfile(my_card_);
    ASSERT_FALSE(profile_llamasticot.get());

    auto profile_llama1 = mapper6_->buildProfile(my_card2_);
    ASSERT_TRUE(profile_llama1.get());

    auto profile_llama2 = mapper6_->buildProfile(my_pin_);
    ASSERT_FALSE(profile_llama2.get());
}

TEST_F(AuthFileMapperTest, TestCredentialSchedule)
{
    mapper7_->mapToUser(my_card_);
    mapper7_->mapToUser(my_pin_);
    auto profile = mapper7_->buildProfile(my_card_);

    // this maps to the same user, but use a credential
    // with additional schedules.
    auto profile_magic_cred = mapper7_->buildProfile(my_pin_);

    ASSERT_TRUE(profile.get());
    ASSERT_TRUE(profile_magic_cred.get());

    // User permissions.
    ASSERT_TRUE(profile->isAccessGranted(date_monday_12_00, nullptr));
    ASSERT_TRUE(profile->isAccessGranted(date_monday_16_31, nullptr));

    ASSERT_TRUE(profile_magic_cred->isAccessGranted(date_monday_12_00, nullptr));
    ASSERT_TRUE(profile_magic_cred->isAccessGranted(date_monday_16_31, nullptr));
    // Not allowed
    ASSERT_FALSE(profile->isAccessGranted(date_sunday_18_50, nullptr));
    ASSERT_FALSE(profile->isAccessGranted(date_wednesday_23_42, doorB_));
    ASSERT_FALSE(profile->isAccessGranted(date_wednesday_23_42, doorC_));
    ASSERT_FALSE(profile->isAccessGranted(date_wednesday_23_42, nullptr));

    ASSERT_FALSE(profile_magic_cred->isAccessGranted(date_sunday_18_50, nullptr));
    ASSERT_FALSE(profile_magic_cred->isAccessGranted(date_wednesday_23_42, doorB_));
    ASSERT_FALSE(profile_magic_cred->isAccessGranted(date_wednesday_23_42, doorC_));
    ASSERT_FALSE(profile_magic_cred->isAccessGranted(date_wednesday_23_42, nullptr));

    // Per credentials schedule
    ASSERT_TRUE(profile_magic_cred->isAccessGranted(date_wednesday_23_42, doorA_));
    ASSERT_TRUE(profile_magic_cred->isAccessGranted(date_thursday_14_00, nullptr));
    // if we used the "normal" those default permission shall fail
    ASSERT_FALSE(profile->isAccessGranted(date_wednesday_23_42, doorA_));
    ASSERT_FALSE(profile->isAccessGranted(date_thursday_14_00, nullptr));
}

/**
 * UserID `UNKNOWN_USER` is reserved to prevent confusion in the log file.
 * Test that the mapper refuse to build when such a user is defined.
 */
TEST_F(AuthFileMapperTest, TestReservedUserID)
{
    ASSERT_THROW(
        {
            std::unique_ptr<IAuthSourceMapper> faulty_mapper(
                new FileAuthSourceMapper(gl_data_path + "AuthFile-9.xml"));
        },
        ModuleException);
    // Nested exception. The original type is a ConfigException.
}
}
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    // gtest shall leave us with our arguments.
    // argv[1] shall be the path to test data file
    assert(argc == 2);
    gl_data_path = std::string(argv[1]) + '/';
    return RUN_ALL_TESTS();
}
