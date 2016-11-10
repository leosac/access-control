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

#include "tools/service/ServiceRegistry.hpp"
#include "gtest/gtest.h"

using namespace Leosac;

namespace Leosac
{
namespace Test
{

struct DummyServiceInterface
{
    virtual bool is_even(int number) const = 0;
};

struct DummyServiceInterface2
{
};

struct DummyServiceImpl : public DummyServiceInterface
{
    virtual bool is_even(int number) const override
    {
        return (number % 2) == 0;
    }
};

TEST(TestRegistry, register_get_service)
{
    ServiceRegistry srv_registry;

    // No service registered for this interface.
    // Will return -1.
    ASSERT_EQ(-1, srv_registry.use_count<DummyServiceInterface>());
    // No service yet, will return null.
    ASSERT_EQ(nullptr, srv_registry.get_service<DummyServiceInterface>());

    auto handle =
        srv_registry.register_service<DummyServiceInterface>(new DummyServiceImpl());

    ASSERT_EQ(0, srv_registry.use_count<DummyServiceInterface>());
    {
        std::shared_ptr<DummyServiceInterface> srv =
            srv_registry.get_service<DummyServiceInterface>();
        ASSERT_EQ(1, srv_registry.use_count<DummyServiceInterface>());
        ASSERT_TRUE(srv->is_even(0));
        ASSERT_FALSE(srv->is_even(1));
    }
    ASSERT_EQ(0, srv_registry.use_count<DummyServiceInterface>());
}

TEST(TestRegistry, unregister_simple)
{
    ServiceRegistry srv_registry;

    auto handle =
        srv_registry.register_service<DummyServiceInterface>(new DummyServiceImpl());
    ASSERT_NE(nullptr, srv_registry.get_service<DummyServiceInterface>());

    ASSERT_TRUE(srv_registry.unregister_service(handle));
    ASSERT_EQ(nullptr, srv_registry.get_service<DummyServiceInterface>());
}

TEST(TestRegistry, unregister_while_used)
{
    ServiceRegistry srv_registry;

    auto handle =
        srv_registry.register_service<DummyServiceInterface>(new DummyServiceImpl());
    ASSERT_NE(nullptr, srv_registry.get_service<DummyServiceInterface>());

    {
        auto srv = srv_registry.get_service<DummyServiceInterface>();
        ASSERT_FALSE(srv_registry.unregister_service(handle));
    }
    ASSERT_TRUE(srv_registry.unregister_service(handle));
}

TEST(TestRegistry, unregister_no_param)
{
    ServiceRegistry srv_registry;

    auto handle =
        srv_registry.register_service<DummyServiceInterface>(new DummyServiceImpl());
    ASSERT_FALSE(srv_registry.unregister_service<DummyServiceInterface2>());
    ASSERT_TRUE(srv_registry.unregister_service<DummyServiceInterface>());
}
}
}
