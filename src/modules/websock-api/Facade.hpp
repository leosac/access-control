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

#include "LeosacFwd.hpp"
#include "modules/websock-api/Messages.hpp"
#include "modules/websock-api/RequestContext.hpp"
#include "modules/websock-api/api/CRUDResourceHandler.hpp"
#include <map>
#include <zmqpp/zmqpp.hpp>

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
/**
 * This object is a facade to interact with the WebSockAPI module.
 *
 * It provides an easy to use API that abstract away the message passing
 * construct necessary to communicate with the module (register handler,
 * send message, ...)
 *
 * The class also provide lightweight message dispatching.
 */
class Facade
{
  public:
    /**
     * Create a Facade to interact with the websocket module.
     */
    Facade(zmqpp::reactor &, CoreUtilsPtr utils);

    /**
     * This constructor is aimed to be used once per module.
     *
     * It will send the REGISTER_MODULE message at construction, to let
     * the websocket module know who it is talking to.
     */
    Facade(zmqpp::reactor &, CoreUtilsPtr utils, const std::string module_name);

    void send_message(const std::string &connection_identifier,
                      const std::string &content);

    void send_message(const std::string &connection_identifier,
                      const ServerMessage &msg);

    /**
     * Register a handler for a single API call.
     * @tparam Callable
     * @param type The name of the handler (= message type)
     */
    template <typename Callable>
    void register_handler(const std::string &type, Callable &&c)
    {
        handlers_[type] = c;
        send_handler_registration_message(type);
    }

    void
    register_crud_handler(const std::string &type,
                          ExternalCRUDResourceHandler::Factory crud_handler_factory);

  private:
    void handle_dealer();

    void send_handler_registration_message(const std::string &type);

    ServerMessage dispatch(const ModuleRequestContext &mrc,
                           const ClientMessage &msg);

    using HandlerMap =
        std::map<std::string,
                 std::function<ServerMessage(const ModuleRequestContext &,
                                             const ClientMessage &)>>;

    HandlerMap handlers_;
    zmqpp::reactor &reactor_;
    CoreUtilsPtr utils_;
    zmqpp::socket dealer_;
};
}
}
}