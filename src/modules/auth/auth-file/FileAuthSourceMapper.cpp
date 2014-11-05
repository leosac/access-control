#include "core/auth/Interfaces/IAuthenticationSource.hpp"
#include "FileAuthSourceMapper.hpp"
#include "core/auth/WiegandCard.hpp"
#include <iostream>

using namespace Leosac::Module::Auth;
using namespace Leosac::Auth;

FileAuthSourceMapper::FileAuthSourceMapper(const std::string &auth_file)
{

}

void FileAuthSourceMapper::visit(WiegandCard *src)
{
    std::cout << "VISITING" << std::endl;
}
