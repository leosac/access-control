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

#pragma once

#include <type_traits>

namespace Leosac
{
namespace Tools
{

/**
 * Base class for visitor, should not be used directly.
 *
 * To write a visitor that can visit an object of type `T`, subclass
 * `Visitor<T>`.
 */
class BaseVisitor
{
  public:
    virtual ~BaseVisitor() = default;
};

class IVisitable;

/**
 * A Visitor object.
 *
 * By subclassing this class, an object will be able to visit objects
 * of type `T`.
 *
 * For this to work, the `T` object needs to subclass IVisitable and use
 * the `MAKE_VISITABLE()` macro.
 *
 * The Visitor<T> class declare and define 2 `visit()` methods. One for
 * `const Visitable` and one for non-const Visitable. By default the non-const
 * version forward the call to the const version.
 */
template <typename T>
class Visitor : public virtual BaseVisitor
{
  public:
    using VisitableT = std::remove_reference_t<std::remove_const_t<T>>;

    // While correct, this forces additional includes.
    // static_assert(std::is_base_of<IVisitable, VisitableT>::value,
    //             "Trying to implement a visitor for a type that doesn't implement "
    //           "IVisitable.");

    virtual void visit(const VisitableT &)
    {
    }
    virtual void visit(VisitableT &visitable)
    {
        visit(const_cast<const VisitableT &>(visitable));
    }
};
}
}
