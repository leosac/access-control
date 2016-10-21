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

#include "WSServer.hpp"
#include "core/auth/AuthFwd.hpp"
#include "modules/BaseModule.hpp"
#include "tools/db/db_fwd.hpp"

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
/**
 * A module that provide a websocket interface to Leosac.
 *
 * The websocket API is designed to be exposed to the world. It can be used
 * to implement various UI to Leosac.
 *
 * The module also provide a key utility to other modules:
 *    + It can passthrough websocket traffic, effectively giving the ability to
 *      any module to expose a websocket API for itself.
 *
 * To provide this features, the module binds a ROUTER socket
 * at "inproc://SERVICE.WEBSOCKET". Other modules are expected to connect to this
 * socket and issue command.
 *
 * Todo: This doc needs to be improved.
 *
 * ### Supported commands.
 *
 * #### REGISTER_MODULE
 *
 * Frame1: Module name
 *
 * This register the module name to the websocket module. It will allows
 * websocket handler to communicate with this module. A use case is
 * the forwarding of AccessPoint related request to their implementation
 * modules.
 *
 *
 * #### REGISTER_HANDLER.
 *
 * Frame1: "REGISTER_HANDLER"
 * Frame2: "MY_HANDLER_NAME"
 *
 * This command let the module that the caller wants to receive message
 * whose `type` is "MY_HANDLER_NAME". From this point on, all message whose
 * type is "MY_HANDLER_NAME" will be forward to the module that registered the
 * handler.
 *
 * The websocket module will either response "OK" ok "KO" depending on whether or
 * not registration was successful.
 *
 * #### SEND_MESSAGE
 *
 * Frame1: "SEND_MESSAGE"
 * Frame2: `Connection identifier`
 * Frame3: JSON TEXT CONTENT.
 *
 *
 * The module will also emit messages (when receiving message for a registered
 * handler).
 *
 * The internal PULL socket must be connected only by the websocketpp thread.
 * All message received will be forwarded as is, unless the first frame is
 * "SEND_TO_MODULE". In that case, the message will be delivered to the appropriate
 * module. Or assert if not found.
 *
 * #### Forwarding (sent by WSServer object)
 * Frame1: Client identifier (module that registere the handler)
 * Frame2: Audit Entry ID
 * Frame3: `Connection identifier`
 * Frame4: JSON TEXT CONTENT.
 *
 */
class WebSockAPIModule : public BaseModule
{
  public:
    WebSockAPIModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                     const boost::property_tree::ptree &cfg, CoreUtilsPtr utils);

    ~WebSockAPIModule() = default;

    virtual void run() override;

    /**
     * This module explicitly expose CoreUtils to other
     * object in the module.
     */
    CoreUtilsPtr core_utils();

  private:
    /**
     * A message arrived on the router socket.
     */
    void handle_router();

    /**
     * A message arrived on the pull socket.
     *
     * Message received on this socket are supposed to be forwarded
     * to a client of the router socket.
     */
    void handle_pull();

    /**
     * Port to bind the websocket endpoint.
     */
    uint16_t port_;

    /**
     * IP address of the interface to listen on.
     */
    std::string interface_;

    /**
     * A ROUTER socket that will be used to communicate
     * with other module.
     *
     * This socket binds to SERVICE.WEBSOCKET
     */
    std::unique_ptr<zmqpp::socket> router_;

    /**
     * PULL socket that receives message from the WSServer object.
     */
    std::unique_ptr<zmqpp::socket> pull_;

    /**
     * Our websocket server object.
     */
    std::unique_ptr<WSServer> wssrv_;

    std::map<std::string, std::string> module_name_client_id_;
};
}
}
}
