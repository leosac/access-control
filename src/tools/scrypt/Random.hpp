/*
    Copyright (C) 2014-2022 Leosac

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

#include <cstdint>
#include <mutex>
#include <vector>

using ByteVector = std::vector<uint8_t>;

class Random
{
  public:
    /**
     * Generate `n` random bytes.
     *
     * @param n Size of random data.
     * @return Random data.
     */
    static ByteVector GetBytes(size_t n);

    /**
     * Return a random ASCII string.
     */
    static std::string GetASCII(size_t n);

  private:
    /**
     * OpenSSL's random number generation is not thread-safe.
     */
    static std::mutex mutex_;
};
