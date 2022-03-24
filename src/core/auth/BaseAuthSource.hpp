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

#include "core/auth/Interfaces/IAuthenticationSource.hpp"
#include "core/auth/ValidityInfo.hpp"
#include <memory>
#include <vector>

namespace Leosac
{
namespace Auth
{
class BaseAuthSource;
using BaseAuthSourcePtr = std::shared_ptr<BaseAuthSource>;

class BaseAuthSource : public IAuthenticationSource
{
  public:
    MAKE_VISITABLE();

    BaseAuthSource(const std::string &id = "");
    virtual ~BaseAuthSource()            = default;
    virtual void addAuthSource(IAuthenticationSourcePtr source) override;

    virtual void owner(UserPtr user) override;

    virtual UserPtr owner() const override;

    /**
    * This implementation simply return the profile associated with
    * the credential, or null if not available.
    */
    virtual IAccessProfilePtr profile() const override;

    virtual void profile(IAccessProfilePtr p) override;

    virtual const std::string &name() const override;

    /**
    * Set a name for this auth source
    */
    void name(const std::string &n);

    virtual std::string to_string() const override;

    virtual const std::string &id() const override;

    void id(const std::string &cred_id) override;

    virtual const ValidityInfo &validity() const override;

    virtual void validity(const ValidityInfo &c) override;

  protected:
    /**
    * Which user this auth source maps to. May be null
    */
    UserPtr owner_;

    /**
    * Underlying auth source.
    */
    std::vector<IAuthenticationSourcePtr> subsources_;

    /**
    * Name of the source (generally the module / device that sent it)
    */
    std::string source_name_;

    /**
    * Credentials ID.
    */
    std::string id_;

    ValidityInfo validity_;

    IAccessProfilePtr profile_;
};
}
}
