#include <gtest/gtest.h>
#include <chrono>
#include <tools/unixshellscript.hpp>
#include <core/auth/Interfaces/IAuthSourceMapper.hpp>
#include "modules/auth/auth-file/FileAuthSourceMapper.hpp"

using namespace Leosac::Auth;
using namespace Leosac::Module::Auth;
namespace Leosac
{
    namespace Test
    {
        class AuthFileMapperTest : public ::testing::Test
        {
        public:
            AuthFileMapperTest()
            {
                mapper_ = new FileAuthSourceMapper("bla");
            }

            ~AuthFileMapperTest()
            {
                delete mapper_;
            }

            IAuthSourceMapper *mapper_;
        };


        TEST_F(AuthFileMapperTest, SimpleMapping)
        {

        }
    }
}
/*

int main(int ac, char **av)
{
    return 0;
}

*/
