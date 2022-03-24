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

#include "tools/Visitor.hpp"
#include "tools/IVisitable.hpp"
#include "gtest/gtest.h"

using namespace Leosac;
using namespace Leosac::Tools;

namespace Leosac
{
namespace Test
{

struct DummyVisitable : public virtual IVisitable
{
    DummyVisitable() = default;
    MAKE_VISITABLE();
};

struct DummyVisitor : public Visitor<DummyVisitable>
{
    DummyVisitor()
    {
    }

    void visit(DummyVisitable &) override
    {
        visited_non_const_ = true;
    }

    void visit(const DummyVisitable &) override
    {
        visited_const_ = true;
    }

    bool visited_non_const_{false};
    bool visited_const_{false};
};

TEST(TestVisitor, simple_non_const)
{
    DummyVisitable visitable;
    DummyVisitor visitor;
    visitable.accept(visitor);
    ASSERT_TRUE(visitor.visited_non_const_);
    ASSERT_FALSE(visitor.visited_const_);
}

TEST(TestVisitor, simple_const)
{
    const DummyVisitable visitable;
    DummyVisitor visitor;
    visitable.accept(visitor);
    ASSERT_TRUE(visitor.visited_const_);
    ASSERT_FALSE(visitor.visited_non_const_);
}

struct DummyConstOnlyVisitor : public Visitor<DummyVisitable>
{
    DummyConstOnlyVisitor()
        : visited_const_(false)
    {
    }

    void visit(const DummyVisitable &) override
    {
        visited_const_ = true;
    }

    bool visited_const_;
};

TEST(TestVisitor, const_visit_accept_non_const_visitable)
{
    DummyVisitable visitable;
    DummyConstOnlyVisitor visitor;
    visitable.accept(visitor);
    ASSERT_TRUE(visitor.visited_const_);
}

TEST(TestVisitor, const_visit_accept_const_visitable)
{
    const DummyVisitable visitable;
    DummyConstOnlyVisitor visitor;
    visitable.accept(visitor);
    ASSERT_TRUE(visitor.visited_const_);
}

/**
 * Visitor2 that is ready to visit object
 * of type DummyVisitable, but not of type
 * DummyChildVisitable. Yet, we try to
 * pass it a DummyChildVisitable
 */
struct DummyVisitor2 : public Visitor<DummyVisitable>
{
    DummyVisitor2() = default;

    void visit(const DummyVisitable &) override
    {
        visited_ = true;
    }

    bool visited_{false};
};

struct DummyChildVisitable : public virtual DummyVisitable
{
    DummyChildVisitable() = default;

    MAKE_VISITABLE_FALLBACK(DummyVisitable);
};

TEST(TestVisitor, test_class_hierarchy)
{
    DummyChildVisitable visitable;
    DummyVisitor2 visitor;
    visitable.accept(visitor);
    ASSERT_TRUE(visitor.visited_);
}
}
}
