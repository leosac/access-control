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

#include "MethodHandler.hpp"

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
using json = nlohmann::json;

/**
 * Retrieve an overview of the access permission of users against
 * doors.
 *
 * The overview is a simple TRUE/FALSE regarding the user permission against
 * a door. It doesn't handle timeframe yet.
 *
 * Request:
 *     + No parameter required. This call will return an general overview.
 *
 * Response:
 *     [
 *       {door_id: $SOME_DOOR_ID,
 *       user_ids: [1, 4, 24]
 *     ]
 *
 */
class AccessOverview : public MethodHandler
{
  public:
    AccessOverview(RequestContext ctx);

    static MethodHandlerUPtr create(RequestContext);

  protected:
    std::vector<ActionActionParam>
    required_permission(const json &req) const override;

  private:
    virtual json process_impl(const json &req) override;
};
}
}
}
