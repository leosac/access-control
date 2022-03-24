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

#include "core/auth/AuthFwd.hpp"
#include "tools/IVisitable.hpp"
#include <memory>

namespace Leosac
{
namespace Auth
{
class IAuthenticationSource;

using IAuthenticationSourcePtr = std::shared_ptr<IAuthenticationSource>;

/**
* An Auth source is a card id, a pin code, a fingerprint, etc...
*
* Those sources can be combined to provide strong authentication mechanism.
* This class handle this.
*/
class IAuthenticationSource : public Tools::IVisitable
{
  public:
    virtual ~IAuthenticationSource() = default;

    /**
    * Adds a new authentication sources as a subsource of this one.
    */
    virtual void addAuthSource(IAuthenticationSourcePtr source) = 0;

    /**
    * Set the user that map to this source,
    */
    virtual void owner(UserPtr user) = 0;

    /**
    * Retrieve the user that map to this source.
    */
    virtual UserPtr owner() const = 0;

    /**
    * Return (or generate) an Access Profile that make sense
    * based on the data available in this AuthenticationSource object.
    *
    * In addition to the user's profile, this profile will later be used
    * to chose between granting and denying access to an end user.
    */
    virtual IAccessProfilePtr profile() const = 0;

    /**
    * Set an access profile for this credentials.
    * This profile will be added to the user's profile
    * when granting/denying acccess.
    */
    virtual void profile(IAccessProfilePtr p) = 0;

    /**
    * Return the name of the source. This could be the name of the name of the module
    * or device that sent the message that lead to this object's creation.
    */
    virtual const std::string &name() const = 0;

    /**
    * Returns a string representation of the authentication source content.
    */
    virtual std::string to_string() const = 0;

    /**
    * Returns the ID of the credentials.
    * It is possible that this ID be empty.
    */
    virtual const std::string &id() const = 0;

    virtual void id(const std::string &new_id) = 0;

    /**
    * Get the object that store info about the credential's validity
    */
    virtual const ValidityInfo &validity() const = 0;

    /**
    * Update the validity of the credential.
    */
    virtual void validity(const ValidityInfo &c) = 0;
};
}
}
