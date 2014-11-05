#include <gtest/gtest.h>
#include <chrono>
#include <tools/unixshellscript.hpp>
#include <core/auth/Interfaces/IAuthSourceMapper.hpp>
#include "modules/auth/auth-file/FileAuthSourceMapper.hpp"

// path to test-data file.
// this come from command line (see CmakeLists.txt)
std::string gl_data_path;

using namespace Leosac::Auth;
using namespace Leosac::Module::Auth;
namespace Leosac
{
    namespace Test
    {
        /**
        * @note This test suite use the AuthFile-1.xml file.
        */
        class AuthFileMapperTest : public ::testing::Test
        {
        public:
            AuthFileMapperTest() :
                    my_card_(new WiegandCard("aa:bb:cc:dd", 32)),
                    my_card2_(new WiegandCard("cc:dd:ee:ff", 32))
            {
                mapper_ = new FileAuthSourceMapper(gl_data_path + "AuthFile-1.xml");
            }

            ~AuthFileMapperTest()
            {
                delete mapper_;
            }

            IAuthSourceMapper *mapper_;
            IAuthenticationSourcePtr my_card_;
            IAuthenticationSourcePtr my_card2_;
        };


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
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    // gtest shall leave us with our arguments.
    // argv[1] shall be the path to test data file
    assert(argc == 2);
    gl_data_path = std::string(argv[1]) + '/';
    std::cout << "GLDATA = " << gl_data_path << std::endl;
    return RUN_ALL_TESTS();
}
