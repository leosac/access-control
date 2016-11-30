/*
    Copyright (C) 2014-2016 Leosac

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
#include "core/audit/IWSAPICall.hpp"

namespace Leosac
{
namespace Audit
{
/**
 * Provides the implementation of IWSAPICall.
 */
#pragma db object polymorphic callback(odb_callback)
class WSAPICall : virtual public IWSAPICall, public AuditEntry
{
  private:
    WSAPICall();

    friend class Factory;

    static WSAPICallPtr create(const DBPtr &database);

  public:
    virtual ~WSAPICall() = default;

    virtual void method(const std::string &string) override;

    virtual void uuid(const std::string &string) override;

    virtual void status_code(APIStatusCode code) override;

    virtual void status_string(const std::string &string) override;

    virtual void source_endpoint(const std::string &string) override;

    virtual void request_content(const std::string &string) override;

    virtual void response_content(const std::string &string) override;

    virtual void database_operations(uint16_t nb_operation) override;

    virtual const std::string &method() const override;

    virtual const std::string &uuid() const override;

    virtual APIStatusCode status_code() const override;

    virtual const std::string &status_string() const override;

    virtual const std::string &source_endpoint() const override;

    virtual std::string generate_description() const override;

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

    /**
     * Count the number of database operation that happened while
     * processing this API call.
     */
    uint16_t database_operations_;

  private:
    friend class odb::access;
};
}
}

#ifdef ODB_COMPILER
#include "core/auth/User.hpp"
#endif
