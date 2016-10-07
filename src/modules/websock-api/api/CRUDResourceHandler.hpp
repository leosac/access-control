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

#include "RequestContext.hpp"
#include "WSSecurityContext.hpp"
#include "WebSockFwd.hpp"
#include <json.hpp>
#include <vector>

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
using json = nlohmann::json;

class CRUDResourceHandler
{
  protected:
    CRUDResourceHandler(RequestContext ctx);

  public:
    enum class Verb
    {
        CREATE,
        READ,
        UPDATE,
        DELETE
    };
    using Factory = CRUDResourceHandlerUPtr (*)(RequestContext);

    static CRUDResourceHandlerUPtr instanciate(RequestContext);

    json process(const ClientMessage &msg);

  protected:
    /**
     * A pair of Action and a generic ActionParam union.
     */
    using ActionActionParam =
        std::pair<SecurityContext::Action, SecurityContext::ActionParam>;

    RequestContext ctx_;

    /**
     * Helper function that returns the security context.
     */
    WSSecurityContext &security_context();

  private:
    virtual std::vector<ActionActionParam>
    required_permission(Verb verb, const json &req) const = 0;

    virtual json create_impl(const json &req) = 0;

    virtual json read_impl(const json &req) = 0;

    virtual json update_impl(const json &req) = 0;

    virtual json delete_impl(const json &req) = 0;

    Verb verb_from_request_type(const std::string &);

    void enforce_permission(const std::vector<ActionActionParam> &);
};
}
}
}
