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

#include "MethodHandler.hpp"

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
using json = nlohmann::json;

/**
 * Update information about a group, or create a new group.
 *
 * Request:
 *     + `group_id`: The group_id of the group we want to update.
 *       If this is `0`, this means we want to create a new group.
 *     + `attributes`: A dictionnary of a groups' attributes.
 *
 *     Accepted attributes:
 *         + name
 *         + description
 *
  * Response:
 *     + ...
 */
class GroupPut : public MethodHandler
{
  public:
    GroupPut(RequestContext ctx);

    static MethodHandlerUPtr create(RequestContext);

  private:
    virtual json process_impl(const json &req) override;

    json create_group(const std::string &name, const std::string &desc);
};
}
}
}
