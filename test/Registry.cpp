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

#include "tools/registry/Registry.hpp"
#include "gtest/gtest.h"

using namespace Leosac;

namespace Leosac
{
namespace Test
{

TEST(TestRegistry, valid_get_set)
{
    Registry<std::string> r;
    r.set("test", 42);
    r.set("test0", -42);
    r.set("test1", std::string("a_string"));

    ASSERT_EQ(42, r.get<int>("test"));
    ASSERT_EQ(-42, r.get<int>("test0"));
    ASSERT_EQ("a_string", r.get<std::string>("test1"));
}

TEST(TestRegistry, invalid_get_set)
{
    Registry<std::string> r;
    r.set("test", 42);
    r.set("test0", -42);
    r.set("test1", "a_string");

    ASSERT_THROW(r.get<int>("this_key_doesnt_exists"), RegistryKeyNotFoundException);
}

TEST(TestRegistry, expiration)
{
    Registry<int> r;
    auto expire_at = Registry<int>::Clock::now() + std::chrono::milliseconds(3000);

    r.set(1, 3, expire_at);
    r.set(2, 6, expire_at);
    r.set(3, 9, expire_at);

    r.purge();
    ASSERT_EQ(3, r.get<int>(1));
    r.purge();
    ASSERT_EQ(6, r.get<int>(2));
    r.purge();
    ASSERT_EQ(9, r.get<int>(3));

    std::this_thread::sleep_for(std::chrono::milliseconds(3001));
    r.purge();
    ASSERT_THROW(r.get<int>(1), RegistryKeyNotFoundException);
    ASSERT_THROW(r.get<int>(2), RegistryKeyNotFoundException);
    ASSERT_THROW(r.get<int>(3), RegistryKeyNotFoundException);
}

TEST(TestRegistry, auto_expiration)
{
    Registry<int> r;
    auto expire_at = Registry<int>::Clock::now() + std::chrono::milliseconds(3000);

    r.set(1, 3, expire_at);
    r.set(2, 6, expire_at);
    r.set(3, 9, expire_at);

    ASSERT_EQ(3, r.get<int>(1));
    ASSERT_EQ(6, r.get<int>(2));
    ASSERT_EQ(9, r.get<int>(3));

    std::this_thread::sleep_for(std::chrono::milliseconds(3001));

    // Object still here, no autopurge yet.
    ASSERT_NO_THROW(r.get<int>(1));
    ASSERT_NO_THROW(r.get<int>(2));
    ASSERT_NO_THROW(r.get<int>(3));

    // Setting some value will trigger purge().
    r.set(42, 10);
    ASSERT_THROW(r.get<int>(1), RegistryKeyNotFoundException);
    ASSERT_THROW(r.get<int>(2), RegistryKeyNotFoundException);
    ASSERT_THROW(r.get<int>(3), RegistryKeyNotFoundException);
}
}
}
