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

#include "core/auth/AuthFwd.hpp"
#include "core/credentials/CredentialFwd.hpp"
#include "tools/IVisitable.hpp"
#include "tools/ToolsFwd.hpp"
#include <memory>

namespace Leosac
{
namespace Cred
{
/**
 * Base interface for credential objects.
 */
class ICredential : public virtual Tools::IVisitable
{
  public:
    MAKE_VISITABLE();

    /**
     * Retrieve the identifier of the credential.
     */
    virtual CredentialId id() const = 0;

    /**
     * Manually set the identifier of a credential.
     *
     * This should generally not be use when database interaction
     * may happen.
     */
    virtual void id(const CredentialId &new_id) = 0;

    /**
     * Retrieve the owner of the credential.
     */
    virtual Auth::UserLPtr owner() const = 0;

    /**
     * Returns the `id` of the owner, or 0 if there is
     * no owner (or the owner has no id).
     */
    virtual Auth::UserId owner_id() const = 0;

    /**
     * Set a new owner for the credential.
     */
    virtual void owner(Auth::UserLPtr) = 0;

    /**
     * An alias for the credential.
     */
    virtual std::string alias() const = 0;

    /**
     * Set the alias for the credential.
     */
    virtual void alias(const std::string &) = 0;

    /**
     * An optional description / notes for the credential.
     */
    virtual std::string description() const = 0;

    /**
     * Set a description for this credential.
     */
    virtual void description(const std::string &) = 0;

    /**
     * Provide the validity info object to the credential.
     */
    virtual void validity(const Auth::ValidityInfo &) = 0;

    /**
     * Retrieve validity status from the credential.
     */
    virtual const Auth::ValidityInfo &validity() const = 0;

    /**
     * Retrieve the lazy_weak_ptr to ScheduleMapping that map
     * this credential.
     */
    virtual std::vector<Tools::ScheduleMappingLWPtr>
    lazy_schedules_mapping() const = 0;

    /**
     * Credentials are "optimistic" object (wrt ODB). This means they
     * carry a `version` field. This method returns the version.
     */
    virtual size_t odb_version() const = 0;
};
}
}
