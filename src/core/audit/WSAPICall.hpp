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
 * An Audit entry.
 *
 * Leosac keeps an audit trail. This serves multiple purposes:
 *     + Additional security
 *     + Keep track of changes in order to generate differential configuration
 *     update to door devices.
 *
 * The audit log is sequential.
 */
#pragma db object polymorphic
class WSAPICall : public AuditEntry
{
  public:
    WSAPICall() = default;

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

#pragma db not_null
    std::string status_string_;

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
