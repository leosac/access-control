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
#include "core/SecurityContext.hpp"
#include <json.hpp>

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{

using json = nlohmann::json;

/**
 * The base class for API method handler implementation.
 *
 * The class defines the very simple interface that may be implemented
 * by the various API method implementation.
 *
 * Each subclass should provide a static method to instanciate a handler
 * object of the subclass type.
 */
class MethodHandler
{
  public:
    using Factory = MethodHandlerUPtr (*)(RequestContext);

    MethodHandler(RequestContext ctx)
        : ctx_(ctx)
    {
    }

    virtual ~MethodHandler() = default;

    /**
     * The public `process()` method.
     *
     * It uses Non Virtual Interface pattern and will forward
     * the call to the `process_impl()` method.
     */
    json process(const ClientMessage &msg);

    /**
     * An example method that should be implemented in all subclasses.
     */
    static MethodHandlerUPtr create(RequestContext)
    {
        return nullptr;
    }

    WSSecurityContext &security_context();

  private:
    /**
     * The API method implementation.
     *
     * @return A json object that will be assigned to the `content` key
     * in the JSON message sent to the client.
     */
    virtual json process_impl(const json &req) = 0;

  protected:
    using ActionActionParam =
        std::pair<SecurityContext::Action, SecurityContext::ActionParam>;

    /**
     * Return a list of "Action" / "ActionParam" that must pass before
     * the request is processed.
     */
    virtual std::vector<ActionActionParam>
    required_permission(const json &req) const = 0;

    RequestContext ctx_;
};
}
}
}
