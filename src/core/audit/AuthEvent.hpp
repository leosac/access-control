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

#include "AuditEntry.hpp"
#include "IAuthEvent.hpp"

namespace Leosac
{
namespace Audit
{
/**
 * Provides an implementation of IAuthEvent.
 */
#pragma db object polymorphic callback(odb_callback)
class AuthEvent : virtual public IAuthEvent, public AuditEntry
{
  private:
    AuthEvent();

    friend class Factory;

    static std::shared_ptr<AuthEvent>
    create(const DBPtr &database, Cred::ICredentialPtr credential, const std::string& door, AuditEntryPtr parent = nullptr);

  public:
    virtual ~AuthEvent() = default;

    static std::shared_ptr<AuthEvent> create_empty();

    virtual void credential(Cred::ICredentialPtr cred) override;

    virtual std::string credential_raw() const override;

    virtual Cred::CredentialId credential_id() const override;

    virtual void door(const std::string& d) override;

    virtual std::string door() const override;

    virtual std::string generate_description() const override;

  public:
    /**
     * Generate a short description for the triggering credential.
     */
    std::string generate_credential_description() const;

  private:
    std::string cred_raw_;

#pragma db on_delete(set_null)
    Cred::CredentialLWPtr cred_;

    Cred::CredentialId cred_id_;

    std::string door_;

    friend class odb::access;
};
}
}

#ifdef ODB_COMPILER
#include "core/auth/Door.hpp"
#endif
