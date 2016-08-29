/*
    Copyright (C) 2014-2016 Islog

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

#include "Token.hpp"
#include "core/auth/User.hpp"

using namespace Leosac;
using namespace Leosac::Auth;

const std::string &Token::token() const
{
    return token_;
}

bool Token::is_valid() const
{
    auto now = boost::posix_time::second_clock::local_time();
    return expiration_ >= now;
}

UserPtr Token::owner() const
{
    return owner_;
}

boost::posix_time::ptime Token::expiration() const
{
    return expiration_;
}
