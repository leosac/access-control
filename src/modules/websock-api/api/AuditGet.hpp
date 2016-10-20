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
 * Query the audit log.
 *
 * Request:
 *     + enabled_types: An array of string representing the complete
 *       C++ type of the object.
 *       Possible types are:
 *           + Leosac::Audit::UserEvent
 *           + Leosac::Audit::WSAPICall
 *           + ...
 *       If enabled type is not present, returns all types.
 *     + p: Page number
 *     + ps: Page size
 *
 * Response:
 *     + data: [JSON API data]
 *     + meta:
 *          + count: The number of entries that match the request.
 *          + totalPage: The number of page for to retrieve all items that match
 *            the request.
 */
class AuditGet : public MethodHandler
{
  public:
    AuditGet(RequestContext ctx);

    static MethodHandlerUPtr create(RequestContext);

  protected:
    std::vector<ActionActionParam>
    required_permission(const json &req) const override;

  private:
    virtual json process_impl(const json &req) override;
    std::string build_request_string(const json &req, int page, int page_size) const;

    /**
     * Build the "WHERE typeid IN (...)" string based on the enabled types.
     */
    std::string build_in_clause(const json &req) const;

    /**
     * Check that a given string representing an audit type
     * is sane.
     */
    bool is_stringtype_sane(const std::string &str) const;
};
}
}
}
