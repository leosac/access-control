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

#include "tools/Visitor.hpp"
#include <cassert>

namespace Leosac
{
namespace Tools
{

/**
* Base class to make an object visitable.
 *
 * An type wishing to be visitable can simply inherit from this
 * class and call the `MAKE_VISITABLE()` macro in a public section of
 * its definition.
 *
 * The visitor/visitable infrastructure is NOT statically checked. What
 * this means is that `accept()`ing a visitor that is not able to visit
 * us will result in an assert.
 *
* @see Leosac::Tools::Visitor
*/
class IVisitable
{
  public:
    /**
     * Accept a visitor that may mutate `this`.
     */
    virtual void accept(::Leosac::Tools::BaseVisitor &) = 0;

    /**
     * Accept a visitor that will not mutate `this`.
     */
    virtual void accept(::Leosac::Tools::BaseVisitor &) const = 0;

  protected:
    template <class T>
    static void visitor_dispatch(T &visited, BaseVisitor &visitor)
    {
        using VisitedT = std::remove_reference_t<std::remove_const_t<T>>;
        if (Visitor<VisitedT> *p = dynamic_cast<Visitor<VisitedT> *>(&visitor))
        {
            p->visit(visited);
            return;
        }
        else
        {
            visitor.cannot_visit(static_cast<const IVisitable &>(visited));
        }
    }

/**
 * Provide the object calling this macro in its definition with an
 * `accept()` method that will accept `BaseVisitor` client.
 *
 * Those visitors will be dispatched accordingly by the
 * `visitor_dispatch()` static method.
 */
#define MAKE_VISITABLE()                                                            \
    virtual void accept(::Leosac::Tools::BaseVisitor &v) override                   \
    {                                                                               \
        visitor_dispatch(*this, v);                                                 \
    }                                                                               \
    virtual void accept(::Leosac::Tools::BaseVisitor &v) const override             \
    {                                                                               \
        visitor_dispatch(*this, v);                                                 \
    }
};
}
}
