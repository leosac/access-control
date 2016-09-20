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

#include "core/SecurityContext.hpp"
#include "core/auth/AuthFwd.hpp"

namespace Leosac
{

namespace Module
{
namespace WebSockAPI
{
/**
 * A SecurityContext object for WebSocket client.
 */
class WSSecurityContext : public SecurityContext
{
  public:
    WSSecurityContext(DBServicePtr dbsrv, Auth::UserId id);

    virtual bool check_permission(Action a, const ActionParam &ap) const override;

  private:
    bool can_read_group(const GroupActionParam &gap) const;

    bool can_administrate_group(const GroupActionParam &gap) const;

    bool can_read_membership(const MembershipActionParam &map) const;

    /**
     * We can retrieve basic user info if:
     *     + We are ourself
     *     + We can list membership of at least one of the target user's group.
     */
    bool can_read_user(const UserActionParam &uap) const;

    bool can_read_user_detail(const UserActionParam &uap) const;

    bool can_update_user(const UserActionParam &uap) const;

    /**
     * Helper function that returns true if the user is administrator.
     * @return
     */
    bool is_admin() const;
    Auth::UserId user_id_;
};
}
}
}
