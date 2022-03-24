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
#include "tools/ToolsFwd.hpp"

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
/**
 * CRUD Handler for schedules.
 *
 * @see See the *_impl() method for each requests parameters/response.
 */
class ScheduleCRUD : public CRUDResourceHandler
{
  private:
    ScheduleCRUD(RequestContext ctx);

  public:
    static CRUDResourceHandlerUPtr instanciate(RequestContext);

  private:
    virtual std::vector<ActionActionParam>
    required_permission(Verb verb, const json &req) const override;

    /**
     * Create a new schedule.
     *
     * Request:
     *     + `attributes`: Various attributes regarding the schedules.
     *
     *     Attributes:
     *         + `name`:
     *         + `description`:
     *         + `timeframes`:
     */
    virtual boost::optional<json> create_impl(const json &req) override;

    virtual boost::optional<json> read_impl(const json &req) override;

    /**
     * Update a schedule object.
     *
     * This call is a bit different than `schedule.create` because it allows
     * for specifying the mapping (ScheduleMapping objects) of the schedule.
     *
     * Request:
     *     + `attributes`: ...
     *     + `mapping`:
     *         [
     *           {
     *              + alias: Alias of this mapping
     *              + groups: [IDs, ...] Ids of groups targeted by the mapping
     *              + users: [IDs, ...] Ditto for users
     *              + credentials: [IDs, ...] Ditto for credentials.
     *            }
     *         ]
     */
    virtual boost::optional<json> update_impl(const json &req) override;

    virtual boost::optional<json> delete_impl(const json &req) override;
};
}
}
}
