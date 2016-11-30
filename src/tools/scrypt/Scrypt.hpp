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

#pragma once

#include <cstdlib>
extern "C" {
#include <libscrypt.h>
}

#include <boost/algorithm/hex.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

struct ScryptParam
{
    bool operator==(const ScryptParam &o) const;

    uint64_t N;
    uint32_t r;
    uint32_t p;
    uint32_t len;
};

struct ScryptResult
{
    bool operator==(const ScryptResult &o) const;
    bool operator!=(const ScryptResult &o) const;

    /**
     * Parameters used to generated the hash.
     */
    ScryptParam p;

    /**
     * Salt used for generation
     */
    std::vector<uint8_t> salt;

    /**
     * The hash
     */
    std::vector<uint8_t> hash;
};

/**
 * Interface to serialize a ScryptResult to
 * a `SerializedType` object.
 */
template <typename SerializedType>
class ScryptResultSerializer
{
  public:
    virtual SerializedType Serialize(const ScryptResult &in) const   = 0;
    virtual ScryptResult UnSerialize(const SerializedType &in) const = 0;

    static std::string Hex(const std::vector<uint8_t> &in)
    {
        std::string res;
        boost::algorithm::hex(in.begin(), in.end(), back_inserter(res));
        return res;
    }

    static std::vector<uint8_t> UnHex(const std::string &in)
    {
        std::vector<uint8_t> res;
        boost::algorithm::unhex(in.begin(), in.end(), back_inserter(res));
        return res;
    }

    static std::vector<uint8_t> ToByteVector(const std::string &in)
    {
        return std::vector<uint8_t>(in.begin(), in.end());
    }
};

class StringScryptResultSerializer : public ScryptResultSerializer<std::string>
{
  public:
    virtual std::string Serialize(const ScryptResult &in) const override
    {
        std::stringstream ss;
        ss << in.p.N << ":";
        ss << in.p.r << ":";
        ss << in.p.p << ":";
        ss << Hex(in.salt) << ":";
        ss << Hex(in.hash);

        return ss.str();
    }

    virtual ScryptResult UnSerialize(const std::string &in) const override
    {
        ScryptResult result;
        std::vector<std::string> bla;
        boost::algorithm::split(bla, in, boost::algorithm::is_any_of(":"));

        if (bla.size() != 5)
            throw std::runtime_error("Cannot unserialize.");

        result.p.N   = std::stoi(bla[0]);
        result.p.r   = std::stoi(bla[1]);
        result.p.p   = std::stoi(bla[2]);
        result.salt  = UnHex(bla[3]);
        result.hash  = UnHex(bla[4]);
        result.p.len = result.hash.size();

        return result;
    }
};

class Scrypt
{
  public:
    /**
     * Wrapper around low-level hash function.
     *
     * @param in vector to hash
     * @param salt Salt to use when hashing.
     * @param hashlen the length of the output hash
     * @return Binary hash
     */
    static ScryptResult Hash(const std::vector<uint8_t> &in,
                             const std::vector<uint8_t> &salt,
                             const ScryptParam &param = default_);

    /**
     * Scrypt hash the `in` byte vector.
     *
     * A random salt is generated.
     *
     * @param in
     * @param param
     * @return A ScryptResult object providing all information about the hash.
     */
    static ScryptResult Hash(const std::vector<uint8_t> &in,
                             const ScryptParam &param = default_);

    /**
     * Verify that the input `in`, when hashed, correspond to the `expected`
     * ScryptResult.
     * @param in
     * @param o
     * @return Wether or not the hash match.
     */
    static bool Verify(const std::vector<uint8_t> &in, const ScryptResult &expected);

  private:
    static ScryptParam default_;
};
