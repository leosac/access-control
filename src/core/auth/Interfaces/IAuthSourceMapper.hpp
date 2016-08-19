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

#include "core/auth/Group.hpp"
#include "core/auth/Interfaces/IAuthenticationSource.hpp"
#include "tools/IVisitor.hpp"

namespace Leosac
{
namespace Auth
{
/**
* Base class to perform abstracted mapping operation over
* various AuthSource object.
*
* It uses the visitor pattern to provide a somewhat low-coupling.
* Each module are free to use their own implementation of IAuthSourceMapper
* with various backend (database, file, ...). Theses shall be able to operate
* on various AuthSource (card, pin code, fingerprint).
*/
class IAuthSourceMapper : public Leosac::Tools::IVisitor
{
  public:
    virtual ~IAuthSourceMapper() = default;

    /**
    * Must map the AuthenticationSource data to a User.
    *
    * This is done by calling the `owner()` method on the auth source
    * object. In case it fails, the field shall be set to `nullptr`.
    *
    * @note The User may be `new`ed.
    */
    virtual void mapToUser(IAuthenticationSourcePtr auth_source) = 0;

    /**
    * Build an AccessProfile object given a AuthenticationSource.
    *
    * How the profile is built (ie, based only on user-id, based on auth source and
    * user-id, group membership, etc) is implementation-defined.
    *
    * The caller expect a valid profile object that will behave correctly.
    */
    virtual IAccessProfilePtr buildProfile(IAuthenticationSourcePtr auth_source) = 0;

    /**
    * Return the groups this mapper is aware of.
    */
    virtual std::vector<Leosac::Auth::GroupPtr> groups() const = 0;
};
}
}
