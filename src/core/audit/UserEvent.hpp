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

#include "AuditEntry.hpp"
#include "IUserEvent.hpp"

namespace Leosac
{
namespace Audit
{
/**
 * Provides an implementation of IUserEvent.
 */
#pragma db object polymorphic callback(odb_callback)
class UserEvent : virtual public IUserEvent, public AuditEntry
{
  private:
    UserEvent() = default;

    friend class Factory;

    static std::shared_ptr<UserEvent>
    create(const DBPtr &database, Auth::UserPtr target_user, AuditEntryPtr parent);

  public:
    virtual ~UserEvent() = default;

    virtual void target(Auth::UserPtr user) override;

    Auth::UserId target_id() const override;

    virtual void before(const std::string &repr) override;

    const std::string &before() const override;

    const std::string &after() const override;

    virtual void after(const std::string &repr) override;

    std::string generate_description() const override;

  private:
    /**
     * Generate a small json-string to describe the
     * target user.
     */
    std::string generate_target_description() const;

#pragma db not_null
    Auth::UserLWPtr target_;

    /**
     * Optional JSON dump of the object before the event took place.
     */
    std::string before_;

    /**
     * Optional JSON dump of the object after the event took place.
     */
    std::string after_;

    friend class odb::access;
};
}
}
