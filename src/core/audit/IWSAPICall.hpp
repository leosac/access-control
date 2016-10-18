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

#include "IAuditEntry.hpp"
#include "core/APIStatusCode.hpp"
#include "tools/db/db_fwd.hpp"

namespace Leosac
{
namespace Audit
{
/**
 * An audit entry dedicated to tracing API call.
 *
 * The various parameters of the API call are saved:
 *    1. Source endpoint
 *    2. API method
 *    3. Request/Response content
 *    4. ...
 */
class IWSAPICall : virtual public IAuditEntry
{
  public:
    MAKE_VISITABLE();
    /**
     * Set the API method that was targeted by the call.
     */
    virtual void method(const std::string &) = 0;

    virtual const std::string &method() const = 0;

    /**
     * Set the UUID of the request.
     */
    virtual void uuid(const std::string &) = 0;

    virtual const std::string &uuid() const = 0;

    /**
     * Set the status_code of the reponse.
     */
    virtual void status_code(APIStatusCode) = 0;

    virtual APIStatusCode status_code() const = 0;

    /**
     * Set the status string of the response.
     */
    virtual void status_string(const std::string &) = 0;

    virtual const std::string &status_string() const = 0;

    /**
     * Set the source endpoint. Generally IP:PORT
     */
    virtual void source_endpoint(const std::string &) = 0;

    virtual const std::string &source_endpoint() const = 0;

    /**
     * The (JSON) content of the request.
     */
    virtual void request_content(const std::string &) = 0;

    /*
     * The (JSON) content of the response.
     */
    virtual void response_content(const std::string &) = 0;

    /**
     * The number of database queries.
     */
    virtual void database_operations(uint16_t nb_operation) = 0;
};
}
}
