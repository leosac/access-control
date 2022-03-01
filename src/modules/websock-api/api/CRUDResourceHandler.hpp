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

#include "core/UserSecurityContext.hpp"
#include "modules/websock-api/RequestContext.hpp"
#include "modules/websock-api/WebSockFwd.hpp"
#include <boost/optional.hpp>
#include <nlohmann/json.hpp>
#include <vector>

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
using json = nlohmann::json;

/**
 * A common interface for CRUD handler.
 */
class ICRUDResourceHandler
{
  public:
    enum class Verb
    {
        CREATE,
        READ,
        UPDATE,
        DELETE
    };
    boost::optional<json> process(const ClientMessage &msg);

  protected:
    /**
     * A pair of Action and a generic ActionParam union.
     */
    using ActionActionParam =
        std::pair<SecurityContext::Action, SecurityContext::ActionParam>;

    /**
     * Helper function that returns the security context.
     */
    virtual UserSecurityContext &security_context() const = 0;

  private:
    virtual std::vector<ActionActionParam>
    required_permission(Verb verb, const json &req) const = 0;

    virtual boost::optional<json> create_impl(const json &req) = 0;

    virtual boost::optional<json> read_impl(const json &req) = 0;

    virtual boost::optional<json> update_impl(const json &req) = 0;

    virtual boost::optional<json> delete_impl(const json &req) = 0;

    void enforce_permission(const std::vector<ActionActionParam> &);

    static Verb verb_from_request_type(const std::string &);
};

/**
 * Base CRUD handler for use within the websocket module.
 */
class CRUDResourceHandler : public ICRUDResourceHandler
{
  protected:
    CRUDResourceHandler(RequestContext ctx);

  public:
    using Factory = CRUDResourceHandlerUPtr (*)(RequestContext);

    static CRUDResourceHandlerUPtr instanciate(RequestContext);

  protected:
    RequestContext ctx_;

    virtual UserSecurityContext &security_context() const override;
};

/**
 * For other module to use.
 */
class ExternalCRUDResourceHandler : public ICRUDResourceHandler
{
  public:
    using Factory = ExternalCRUDResourceHandlerUPtr (*)(ModuleRequestContext);


  protected:
    ExternalCRUDResourceHandler(ModuleRequestContext ctx);

    ModuleRequestContext ctx_;

    virtual UserSecurityContext &security_context() const override;
};
}
}
}
