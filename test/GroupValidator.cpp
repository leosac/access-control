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

#include "core/auth/Group.hpp"
#include "core/auth/User.hpp"
#include "exception/ModelException.hpp"
#include "gtest/gtest.h"

using namespace Leosac;
using namespace Leosac::Auth;

namespace Leosac
{
namespace Test
{

TEST(TestGroup, memberships)
{
    auto user  = std::make_shared<User>("toto");
    auto group = std::make_shared<Group>("my_group");

    auto in_grp = [](UserPtr u, GroupPtr g) {
        auto members = g->members();
        for (const auto &member : members)
        {
            if (member->username() == u->username())
                return true;
        }
        return false;
    };
    ASSERT_FALSE(in_grp(user, group));
    group->member_add(user);
    ASSERT_TRUE(in_grp(user, group));
}

TEST(TestGroupValidator, name_length)
{
    Group g;
    ASSERT_THROW({ g.name("s"); }, ModelException);


    ASSERT_NO_THROW({
        g.name("long_enough");
        GroupValidator::validate(g);
    });

    ASSERT_THROW(
        { g.name("this_is_so_long_this_name_is_clearly_to_long_to_be_valid"); },
        ModelException);
}

TEST(TestGroupValidator, name_invalid_char)
{
    Group g;
    ASSERT_NO_THROW({ g.name("aaaa"); });
    ASSERT_THROW({ g.name("aaa$"); }, ModelException);
    ASSERT_THROW({ g.name("aaa!"); }, ModelException);
    ASSERT_THROW({ g.name("aaa^"); }, ModelException);
    ASSERT_THROW({ g.name("aaa^"); }, ModelException);
    ASSERT_THROW({ g.name("aaaä"); }, ModelException);
    ASSERT_THROW({ g.name("aaaé"); }, ModelException);
    ASSERT_THROW({ g.name("aaa aaa"); }, ModelException);
}
}
}
