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

#include "tools/version.hpp"
#include "gtest/gtest.h"

using namespace Leosac::Tools;

namespace Leosac
{
namespace Test
{

TEST(VersionTest, buildVersionString)
{
    EXPECT_EQ("1.2.42", Version::buildVersionString(1, 2, 42));
    EXPECT_EQ("0.0.0", Version::buildVersionString(0, 0, 0));
}

TEST(VersionTest, versionCompare)
{
    EXPECT_EQ(0, Version::versionCompare("0.1.0", "0.1.0"));
    EXPECT_LT(Version::versionCompare("0.1.0", "0.666.0"), 0);
    EXPECT_GT(Version::versionCompare("2.0.0", "1.0.0"), 0);
    EXPECT_GT(Version::versionCompare("3.2.1", "1.2.3"), 0);
}

TEST(VersionTest, isVersionValid)
{
    EXPECT_TRUE(Version::isVersionValid("42.2.85"));
    EXPECT_FALSE(Version::isVersionValid(".42.2.85"));
    EXPECT_FALSE(Version::isVersionValid("42.2."));
    EXPECT_FALSE(Version::isVersionValid("42.200.11.00"));
    EXPECT_FALSE(Version::isVersionValid("11..00"));
}
}
}
