#include "gtest/gtest.h"

#include "tools/version.hpp"

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
