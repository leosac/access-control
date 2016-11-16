/*
    Copyright (C) 2014-2016 Leosac

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

#include "tools/scrypt/Random.hpp"
#include <cassert>
#include <cstddef>
#include <openssl/rand.h>
#include <sstream>
#include <vector>

std::mutex Random::mutex_;

ByteVector Random::GetBytes(size_t n)
{
    if (n == 0)
        return {};
    std::lock_guard<std::mutex> ul(mutex_);
    ByteVector ret(n);

    int rc = RAND_bytes(&ret[0], n);
    assert(rc == 1);
    return ret;
}

std::string Random::GetASCII(size_t n)
{
    if (n == 0)
        return "";

    std::stringstream ss;
    auto bytes = GetBytes(n);
    size_t idx    = 0;
    for (size_t i = 0; i < n; ++i)
    {
        if (idx == bytes.size())
        {
            bytes = GetBytes(n);
            idx   = 0;
        }
        if (isalnum(bytes[idx]))
        {
            ss << bytes[idx];
        }
        idx++;
    }
    assert(ss.str().size() == n);
    return ss.str();
}
