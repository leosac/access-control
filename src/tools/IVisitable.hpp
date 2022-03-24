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
    static bool visitor_dispatch(T &visited, BaseVisitor &visitor,
                                 bool abort_on_failure)
    {
        using VisitedT = std::remove_reference_t<std::remove_const_t<T>>;
        if (Visitor<VisitedT> *p = dynamic_cast<Visitor<VisitedT> *>(&visitor))
        {
            p->visit(visited);
            return true;
        }
        else
        {
            if (abort_on_failure)
            {
                visitor.cannot_visit(static_cast<const IVisitable &>(visited));
            }
            return false;
        }
    }
};

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
        visitor_dispatch(*this, v, true);                                           \
    }                                                                               \
    virtual void accept(::Leosac::Tools::BaseVisitor &v) const override             \
    {                                                                               \
        visitor_dispatch(*this, v, true);                                           \
    }

/**
 * Similar to the MAKE_VISITABLE() macro, with 1 major difference.
 *
 * If the visitor object is not able to visit an object of type (*this),
 * then this->accept() will call PARENT_CLASS->accept() to try again.
 *
 * This allows to us to visit an object mid hierarchy instead of having to
 * handle each concrete implementation.
 */
#define MAKE_VISITABLE_FALLBACK(PARENT_CLASS)                                       \
    virtual void accept(::Leosac::Tools::BaseVisitor &v) override                   \
    {                                                                               \
        if (!visitor_dispatch(*this, v, false))                                     \
        {                                                                           \
            PARENT_CLASS::accept(v);                                                \
        }                                                                           \
    }                                                                               \
    virtual void accept(::Leosac::Tools::BaseVisitor &v) const override             \
    {                                                                               \
        if (!visitor_dispatch(*this, v, false))                                     \
        {                                                                           \
            PARENT_CLASS::accept(v);                                                \
        }                                                                           \
    }
}
}
