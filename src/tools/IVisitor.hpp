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

// Forward declarations
namespace Leosac
{
namespace Auth
{
class BaseAuthSource;
class WiegandCard;
class PINCode;
class WiegandCardPin;
}
}

namespace Leosac
{
namespace Tools
{
class IVisitable;

/**
* Provide an interface from which visitor object can pick methods it
* wants to reimplement.
*
* Each time a new kind of visitor is created, it should be
* added to this class.
* The methods in this interface are actually not pure, because this
* allows visitor to ignore type they are not interested in.
*
* @see Leosac::Tools::IVisitable
*/
class IVisitor
{
  public:
    /**
    * Visit a Visitable object.
    */
    virtual void visit(IVisitable *)
    {
    }

    virtual void visit(Leosac::Auth::BaseAuthSource *)
    {
    }

    virtual void visit(Leosac::Auth::WiegandCard *)
    {
    }

    virtual void visit(Leosac::Auth::PINCode *)
    {
    }

    virtual void visit(::Leosac::Auth::WiegandCardPin *)
    {
    }
};
}
}
