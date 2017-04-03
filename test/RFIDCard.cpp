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

#include "core/credentials/RFIDCard.hpp"
#include "gtest/gtest.h"

using namespace Leosac::Cred;

namespace Leosac
{
namespace Test
{
// Number of bits is ignored
TEST(TestRFIDCard, convert_to_int)
{
    RFIDCard c1("ff:ff:ff:ff", 32);
    ASSERT_EQ(4294967295, c1.to_int());

    RFIDCard c2("aa:bb:cc:dd", 32);
    ASSERT_EQ(2864434397, c2.to_int());

    RFIDCard c3("39:4B:C5:08", 32);
    ASSERT_EQ(961266952, c3.to_int());

    RFIDCard c4("00:00:00:01", 32);
    ASSERT_EQ(1, c4.to_int());

    RFIDCard c5("00:00:00:10", 32);
    ASSERT_EQ(16, c5.to_int());
}

TEST(TestRFIDCard, wiegand_26)
{
    RFIDCard c1("80:80:33:80", 26);
    ASSERT_EQ(103, c1.to_int());

    RFIDCard c2("80:80:33:40", 26);
    ASSERT_EQ(102, c2.to_int());

    RFIDCard c3("80:80:40:00", 26);
    ASSERT_EQ(128, c3.to_int());

    RFIDCard c4("80:80:41:40", 26);
    ASSERT_EQ(130, c4.to_int());
    // Without Wiegand26 format.
    ASSERT_EQ(33685765, c4.to_raw_int());

    RFIDCard c5("80:80:41:80", 26);
    ASSERT_EQ(131, c5.to_int());

    RFIDCard c6("80:80:43:00", 26);
    ASSERT_EQ(134, c6.to_int());
}

/**
 * Test when the reader returns 56 bits (7 bytes).
 *
 * We do nothing special here. Just convert to uint64_t.
 */
TEST(TestRFIDCard, bits_56)
{
    RFIDCard c1("ff:ff:ff:ff", 56);
    ASSERT_EQ(4294967295, c1.to_int());
    ASSERT_EQ(4294967295, c1.to_raw_int());

    RFIDCard c2("aa:bb:cc:dd", 56);
    ASSERT_EQ(2864434397, c2.to_int());

    RFIDCard c3("39:4B:C5:08", 56);
    ASSERT_EQ(961266952, c3.to_int());
    ASSERT_EQ(961266952, c3.to_raw_int());

    RFIDCard c4("00:00:00:01", 56);
    ASSERT_EQ(1, c4.to_int());

    RFIDCard c5("00:00:00:10", 56);
    ASSERT_EQ(16, c5.to_int());
}
}
}
