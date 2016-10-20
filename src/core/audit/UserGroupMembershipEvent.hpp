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
#include "IUserGroupMembershipEvent.hpp"

namespace Leosac
{
namespace Audit
{
/**
 * Provides an implementation of IUserGroupMembershipEvent.
 */
#pragma db object polymorphic callback(odb_callback)
class UserGroupMembershipEvent : virtual public IUserGroupMembershipEvent,
                                 public AuditEntry
{
  private:
    UserGroupMembershipEvent() = default;

    friend class Factory;

    static UserGroupMembershipEventPtr create(const DBPtr &database,
                                              Auth::GroupPtr target_group,
                                              Auth::UserPtr target_user,
                                              AuditEntryPtr parent);

  public:
    virtual ~UserGroupMembershipEvent() = default;

    virtual void target_user(Auth::UserPtr user) override;

    virtual void target_group(Auth::GroupPtr grp) override;

    Auth::UserId target_user_id() const override;

    Auth::GroupId target_group_id() const override;

    std::string generate_description() const override;

  private:
    std::string generate_target_user_description() const;
    std::string generate_target_group_description() const;

#pragma db on_delete(set_null)
    Auth::GroupLWPtr target_group_;

#pragma db on_delete(set_null)
    Auth::UserLWPtr target_user_;

    /**
     * This is equals to target_group_->id().
     *
     * The reason for this data duplication is to keep
     * a trail of which group this event used to refer to.
     */
    Auth::GroupId target_group_id_;

    /**
     * Set to target_user_->id()
     */
    Auth::UserId target_user_id_;

    friend class odb::access;
};
}
}
