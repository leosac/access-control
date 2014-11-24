/**
* @file
* This file contains unit test suite related to the AuthFile module.
*
* @author xaqq
*/

#include <gtest/gtest.h>
#include <chrono>
#include <exception/moduleexception.hpp>
#include "tools/unixshellscript.hpp"
#include "core/auth/Interfaces/IAuthSourceMapper.hpp"
#include "modules/auth/auth-file/FileAuthSourceMapper.hpp"

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
            AuthFileMapperTest() :
                    doorA_(new AuthTarget("doorA")),
                    doorB_(new AuthTarget("doorB")),
                    doorC_(new AuthTarget("doorC")),
                    my_card_(new WiegandCard("aa:bb:cc:dd", 32)),
                    my_card2_(new WiegandCard("cc:dd:ee:ff", 32)),
                    unknown_card_(new WiegandCard("00:00:00:00", 32))
            {
                mapper_ = new FileAuthSourceMapper(gl_data_path + "AuthFile-1.xml");
                mapper2_ = new FileAuthSourceMapper(gl_data_path + "AuthFile-3.xml");

                // initialize date object.
                std::tm date = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                std::tm date2;
                std::tm date3;
                std::time_t time_temp;

                // Monday 12h
                date.tm_year = 114;
                date.tm_mon = 10;
                date.tm_mday = 3;
                date.tm_hour = 12;
                date3 = date2 = date;

                time_temp = std::mktime(&date);
                date_monday_12_00 = std::chrono::system_clock::from_time_t(time_temp);

                // monday 16h31
                date2.tm_hour = 16;
                date2.tm_min = 31;
                time_temp = std::mktime(&date2);
                date_monday_16_31 = std::chrono::system_clock::from_time_t(time_temp);

                // sunday 18h50
                date3.tm_mday = 2;
                date3.tm_hour = 18;
                date3.tm_min = 50;
                time_temp = std::mktime(&date3);
                date_sunday_18_50 = std::chrono::system_clock::from_time_t(time_temp);
            }

            ~AuthFileMapperTest()
            {
                delete mapper_;
            }

            bool is_in_group(const std::string &user_name, const std::string &group_name,
                    IAuthSourceMapper *mapper)
            {
                auto search_lambda = [&](IUserPtr u) -> bool
                {
                    return u->id() == user_name;
                };

                for (const auto &group : mapper->groups())
                {
                    if (group->name() == group_name)
                    {
                        return std::find_if(group->members().begin(),
                                group->members().end(), search_lambda) != group->members().end();
                    }
                }
                return false;
            }

            std::chrono::system_clock::time_point date_monday_12_00;
            std::chrono::system_clock::time_point date_monday_16_31;
            std::chrono::system_clock::time_point date_sunday_18_50;
            AuthTargetPtr doorA_;
            AuthTargetPtr doorB_;
            AuthTargetPtr doorC_;
            IAuthSourceMapper *mapper_;
            // for group related tests.
            IAuthSourceMapper *mapper2_;
            IAuthenticationSourcePtr my_card_;
            IAuthenticationSourcePtr my_card2_;
            IAuthenticationSourcePtr unknown_card_;
        };

        /**
        * Successful mapping
        */
        TEST_F(AuthFileMapperTest, SimpleMapping)
        {
            ASSERT_FALSE(my_card_->owner().get());
            mapper_->mapToUser(my_card_);
            ASSERT_TRUE(my_card_->owner().get());
            ASSERT_EQ("MY_USER", my_card_->owner()->id());

            ASSERT_FALSE(my_card2_->owner().get());
            mapper_->mapToUser(my_card2_);
            ASSERT_TRUE(my_card2_->owner().get());
            ASSERT_EQ("Toto", my_card2_->owner()->id());
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

            ASSERT_TRUE(profile.get());


            ASSERT_TRUE(profile->isAccessGranted(date_monday_12_00, doorA_));
            ASSERT_FALSE(profile->isAccessGranted(date_monday_16_31, doorA_));

            ASSERT_FALSE(profile->isAccessGranted(date_monday_12_00, doorB_));
            ASSERT_TRUE(profile->isAccessGranted(date_monday_16_31, doorB_));

            ASSERT_FALSE(profile->isAccessGranted(date_sunday_18_50, doorA_));
            ASSERT_FALSE(profile->isAccessGranted(date_sunday_18_50, doorB_));
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
            IAccessProfilePtr profile_toto = mapper_->buildProfile(my_card2_);
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
            ASSERT_THROW(std::unique_ptr<IAuthSourceMapper> faulty_mapper(new FileAuthSourceMapper(gl_data_path + "no_file")),
                    ModuleException);
        }

        /**
        * AuthFile-2.xml has invalid content.
        */
        TEST_F(AuthFileMapperTest, InvalidFileContent)
        {
            ASSERT_THROW(
                    {
                        std::unique_ptr<IAuthSourceMapper> faulty_mapper(new FileAuthSourceMapper(gl_data_path + "AuthFile-2.xml"));
                        faulty_mapper->mapToUser(my_card_);
                    }, ModuleException);
        }

        TEST_F(AuthFileMapperTest, TestGroupMapping)
        {
            ASSERT_TRUE(is_in_group("MY_USER", "Admins", mapper2_));
            ASSERT_TRUE(is_in_group("Toto", "Admins", mapper2_));

            ASSERT_FALSE(is_in_group("Useless", "Admins", mapper2_));
            ASSERT_TRUE(is_in_group("Useless", "random_group", mapper2_));
        }

        TEST_F(AuthFileMapperTest, TestGroupPermission)
        {
            mapper2_->mapToUser(my_card_);

            auto profile = mapper2_->buildProfile(my_card_);
            ASSERT_TRUE(profile.get());

            ASSERT_TRUE(profile->isAccessGranted(date_monday_12_00, doorA_));
            ASSERT_FALSE(profile->isAccessGranted(date_monday_16_31, doorA_));

            ASSERT_FALSE(profile->isAccessGranted(date_monday_12_00, doorB_));
            ASSERT_TRUE(profile->isAccessGranted(date_monday_16_31, doorB_));

            ASSERT_FALSE(profile->isAccessGranted(date_sunday_18_50, doorA_));
            ASSERT_FALSE(profile->isAccessGranted(date_sunday_18_50, doorB_));

            ASSERT_TRUE(profile->isAccessGranted(date_sunday_18_50, doorC_));
            ASSERT_FALSE(profile->isAccessGranted(date_monday_16_31, doorC_));
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
