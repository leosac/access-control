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
        class AuthFileMapperTest : public ::testing::Test
        {
        public:
            AuthFileMapperTest() :
                    my_card_(new WiegandCard("aa:bb:cc:dd", 32))
            {
                mapper_ = new FileAuthSourceMapper(gl_data_path + "AuthFile-1");
            }

            ~AuthFileMapperTest()
            {
                delete mapper_;
            }

            IAuthSourceMapper *mapper_;
            IAuthenticationSourcePtr my_card_;
        };


        TEST_F(AuthFileMapperTest, SimpleMapping)
        {
            my_card_->accept(mapper_);
        }
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    // gtest shall leave us with our arguments.
    // argv[1] shall be the path to test data file
    assert(argc == 2);
    gl_data_path = argv[1] + '/';

    return RUN_ALL_TESTS();
}
