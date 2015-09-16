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

#include "gtest/gtest.h"
#include "core/auth/WiegandCard.hpp"

using namespace Leosac::Auth;

namespace Leosac
{
  namespace Test
  {
    // Number of bits is ignored
    TEST(TestWiegandCard, convert_to_int)
    {
      WiegandCard c1("ff:ff:ff:ff", 32);
      ASSERT_EQ(4294967295, c1.to_int());

      WiegandCard c2("aa:bb:cc:dd", 32);
      ASSERT_EQ(2864434397, c2.to_int());

      WiegandCard c3("39:4B:C5:8", 32);
      ASSERT_EQ(60079192, c3.to_int());

      WiegandCard c4("00:00:00:01", 32);
      ASSERT_EQ(1, c4.to_int());

      WiegandCard c5("00:00:00:10", 32);
      ASSERT_EQ(16, c5.to_int());
    }

    TEST(TestWiegandCard, wiegand_26)
    {
      WiegandCard c1("80:80:33:80", 26);
      ASSERT_EQ(103, c1.to_int());

      WiegandCard c2("80:80:33:40", 26);
      ASSERT_EQ(102, c2.to_int());

      WiegandCard c3("80:80:40:00", 26);
      ASSERT_EQ(128, c3.to_int());

      WiegandCard c4("80:80:41:40", 26);
      ASSERT_EQ(130, c4.to_int());

      WiegandCard c5("80:80:41:80", 26);
      ASSERT_EQ(131, c5.to_int());

      WiegandCard c6("80:80:43:00", 26);
      ASSERT_EQ(134, c6.to_int());
    }
  }
}
