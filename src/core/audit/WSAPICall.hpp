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
#include "core/APIStatusCode.hpp"

namespace Leosac
{
namespace Audit
{
/**
 * An audit entry for WebSocket API call.
 *
 * This entry indicates that a Websocket API call took place.
 * It saves the query parameters, the source endpoint, etc.
 */
#pragma db object polymorphic callback(odb_callback)
class WSAPICall : public AuditEntry
{
  public:
    WSAPICall();

    virtual ~WSAPICall() = default;

#pragma db not_null
    std::string api_method_;

/**
 * The UUID of the request.
 */
#pragma db not_null
    std::string uuid_;

/**
 * Status code of the response.
 */
#pragma db not_null
    APIStatusCode status_code_;

/**
 * The status string of the response. May be empty.
 */
#pragma db not_null
    std::string status_string_;

/**
 * The source IP:Port of the client
 * who made the request.
 */
#pragma db not_null
    std::string source_endpoint_;

    /**
     * Copy of the JSON content of the request.
     */
    std::string request_content_;

    /**
     * Copy of the JSON content of the response
     */
    std::string response_content_;

  private:
    friend class odb::access;
};
}
}

#ifdef ODB_COMPILER
#include "core/auth/User.hpp"
#endif
