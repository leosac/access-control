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

#include "ActionActionParam.hpp"
#include "RequestContext.hpp"
#include "core/SecurityContext.hpp"
#include "core/UserSecurityContext.hpp"
#include <nlohmann/json.hpp>

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

    UserSecurityContext &security_context();

    /**
     * Returns a representation of the execution context of this request.
     * This context can be passed to service.
     */
    ExecutionContext exec_context();

  protected:
    /**
     * Return a list of "Action" / "ActionParam" that must pass before
     * the request is processed.
     */
    virtual std::vector<ActionActionParam>
    required_permission(const json &req) const = 0;

    RequestContext ctx_;

  private:
    /**
     * The API method implementation.
     *
     * @return A json object that will be assigned to the `content` key
     * in the JSON message sent to the client.
     */
    virtual json process_impl(const json &req) = 0;
};
}
}
}
