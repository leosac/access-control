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

#pragma once

namespace Leosac
{
namespace Tools
{
class IVisitor;

/**
* Provide an interface that visitable object must implement.
*
* @see Leosac::Tools::IVisitor
*/
class IVisitable
{
  public:
    /**
    * Accept a visitor.
    * @param Visitor shall inherits from the IVisitor interface.
    *
    * @note This must be reimplemented by **all** subclass.
    */
    virtual void accept(IVisitor *) = 0;
};
}
}
