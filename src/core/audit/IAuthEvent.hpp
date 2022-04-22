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

#include "IAuditEntry.hpp"
#include "core/auth/Auth.hpp"
#include "core/credentials/CredentialFwd.hpp"

namespace Leosac
{
namespace Audit
{
/**
 * Interface that describes an Audit object for auth related event.
 */
class IAuthEvent : virtual public IAuditEntry
{
  public:
    MAKE_VISITABLE();

    /**
     * Set the credential which trigerred the event.
     */
    virtual void credential(Cred::ICredentialPtr cred) = 0;

    virtual std::string credential_raw() const  = 0;

    virtual Cred::CredentialId credential_id() const = 0;

    /**
     * Set the AccessPoint that is targeted by the event.
     */
    virtual void access_point_id(Auth::AccessPointId id) = 0;

    virtual Auth::AccessPointId access_point_id() const = 0;

    /**
     * Set the event access status.
     */
    virtual void access_status(Auth::AccessStatus access_status) = 0;

    virtual Auth::AccessStatus access_status() const = 0;
};
}
}
