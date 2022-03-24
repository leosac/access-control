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

#include "api/CRUDResourceHandler.hpp"
#include "core/auth/AuthFwd.hpp"

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
/**
 * CRUD Handler for UserGroupMembership.
 */
class MembershipCRUD : public CRUDResourceHandler
{
  private:
    MembershipCRUD(RequestContext ctx);

  public:
    static CRUDResourceHandlerUPtr instanciate(RequestContext);

  private:
    virtual std::vector<ActionActionParam>
    required_permission(Verb verb, const json &req) const override;

    /**
     * Creating a new UserGroupMembersip: this means someone is joining a group.
     *
     * Request:
     *     + `group_id`: The group to join
     *     + `user_id`: The user that join.
     *     + `rank`: The rank of the user in the group
     *
     * Response:
     *     + ...
     */
    virtual boost::optional<json> create_impl(const json &req) override;

    /**
     * Retrieve information about a group.
     *
     * Request:
     *     + `membership_id`: The id of the membership to look up. Required
     *
     * Response:
     *     + ...
     */
    virtual boost::optional<json> read_impl(const json &req) override;

    virtual boost::optional<json> update_impl(const json &req) override;

    virtual boost::optional<json> delete_impl(const json &req) override;
};
}
}
}
