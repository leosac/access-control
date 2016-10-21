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

#include "api/CRUDResourceHandler.hpp"
#include "core/auth/AuthFwd.hpp"

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
/**
 * CRUD Handler for Groups.
 *
 * @see See the *_impl() method for each requests parameters/response.
 */
class GroupCRUD : public CRUDResourceHandler
{
  private:
    GroupCRUD(RequestContext ctx);

  public:
    static CRUDResourceHandlerUPtr instanciate(RequestContext);

  private:
    virtual std::vector<ActionActionParam>
    required_permission(Verb verb, const json &req) const override;

    /**
     * Create a new group.
     *
     * Request:
     *     + `attributes`: A dictionnary of a groups' attributes.
     *
     *     Accepted attributes:
     *         + name
     *         + description
     *
      * Response:
     *     + ...
     */
    virtual boost::optional<json> create_impl(const json &req) override;

    /**
     * Retrieve information about a group.
     *
     * Request:
     *     + `group_id`: Group we want information about. Required.
     *     A `group_id` of 0 means returns all known groups.
     *
     * For a non-administrator user, it will returns the group the user
     * is a member of.
     *
     * Response:
     *     + ...
     */
    virtual boost::optional<json> read_impl(const json &req) override;

    /**
     * Update information about a group.
     *
     * Request:
     *     + `group_id`: The group_id of the group we want to update.
     *     + `attributes`: A dictionnary of a groups' attributes.
     *
     *     Accepted attributes:
     *         + name
     *         + description
     *
      * Response:
     *     + ...
     */
    virtual boost::optional<json> update_impl(const json &req) override;

    /**
     * Delete a group.
     *
     * Request:
     *     + `group_id`: The group_id of the group we want to delete.
     *
     * Response:
     *     + ...
     */
    virtual boost::optional<json> delete_impl(const json &req) override;

    /**
     * Make sure the group's attribute is valid (by calling
     * GroupValidator) and that the name is unique.
     */
    void validate_and_unique(Auth::GroupPtr grp);
};
}
}
}
