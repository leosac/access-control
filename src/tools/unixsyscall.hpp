/*
    Copyright (C) 2014-2015 Islog

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
* \file unixsyscall.hpp
* \author Thibault Schueller <ryp.sqrt@gmail.com>
* \brief unix syscall helper functions
*/

#ifndef UNIXSYSCALL_HPP
#define UNIXSYSCALL_HPP

#include <string>

namespace Leosac
{
namespace Tools
{
class UnixSyscall
{
    UnixSyscall() = delete;

  public:
    static std::string getErrorString(const std::string &func, int errNo);
};
}
}

#endif // UNIXSYSCALL_HPP
