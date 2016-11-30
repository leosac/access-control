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

#include "tools/scrypt/Scrypt.hpp"
#include "tools/scrypt/Random.hpp"

ScryptParam Scrypt::default_ = {.N = 16384, .r = 8, .p = 1, .len = 64};

ScryptResult Scrypt::Hash(const std::vector<uint8_t> &in,
                          const std::vector<uint8_t> &salt, const ScryptParam &param)
{
    ScryptResult res;
    res.p    = param;
    res.salt = salt;

    std::vector<uint8_t> out;
    out.resize(param.len);

    int ret = libscrypt_scrypt(in.data(), in.size(), salt.data(), salt.size(),
                               param.N, param.r, param.p, out.data(), out.size());
    if (ret != 0)
        throw std::runtime_error("FAILED TO SCRYPT");
    res.hash = out;

    return res;
}

ScryptResult Scrypt::Hash(const std::vector<uint8_t> &in, const ScryptParam &param)
{
    return Hash(in, Random::GetBytes(16), param);
}

bool Scrypt::Verify(const std::vector<uint8_t> &in, const ScryptResult &expected)
{
    auto out = Hash(in, expected.salt, expected.p);
    return out.hash == expected.hash;
}

bool ScryptParam::operator==(const ScryptParam &o) const
{
    return N == o.N && p == o.p && r == o.r && len == o.len;
}

bool ScryptResult::operator==(const ScryptResult &o) const
{
    return p == o.p && salt == o.salt && hash == o.hash;
}

bool ScryptResult::operator!=(const ScryptResult &o) const
{
    return !(*this == o);
}
