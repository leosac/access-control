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

#include "core/auth/AuthFwd.hpp"
#include "core/auth/ValidityInfo.hpp"
#include "core/credentials/ICredential.hpp"
#include <cstddef>

namespace Leosac
{
namespace Cred
{
/**
 * An ODB enabled credential object.
 */
#pragma db object polymorphic optimistic
class Credential : public virtual ICredential
{
  public:
    Credential();
    virtual Auth::UserLPtr owner() const override;

    virtual void owner(Auth::UserLPtr ptr) override;

    virtual std::string alias() const override;

    virtual CredentialId id() const override;

    virtual size_t odb_version() const override;

    virtual Auth::UserId owner_id() const override;

    virtual void alias(const std::string &id) override;

    std::string description() const override;

    void description(const std::string &str) override;

    void validity(const Auth::ValidityInfo &info) override;

    const Auth::ValidityInfo &validity() const override;

  protected:
#pragma db id auto
    CredentialId id_;

    Auth::UserLPtr owner_;

    std::string alias_;

    std::string description_;

    Auth::ValidityInfo validity_;

#pragma db version
    size_t odb_version_;

  private:
    friend class odb::access;
};
}
}

#ifdef ODB_COMPILER
#include "core/auth/User.hpp"
#endif
