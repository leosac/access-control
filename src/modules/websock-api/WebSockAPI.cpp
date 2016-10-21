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

#include "WebSockAPI.hpp"
#include "WSServer.hpp"
#include "core/CoreAPI.hpp"
#include "core/CoreUtils.hpp"
#include "tools/XmlPropertyTree.hpp"
#include <boost/filesystem.hpp>
#include <zmqpp/proxy.hpp>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

WebSockAPIModule::WebSockAPIModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                                   const boost::property_tree::ptree &cfg,
                                   CoreUtilsPtr utils)
    : BaseModule(ctx, pipe, cfg, utils)
{
    port_      = cfg.get<uint16_t>("module_config.port", 8976);
    interface_ = cfg.get<std::string>("module_config.interface", "127.0.0.1");

    pull_ = std::make_unique<zmqpp::socket>(ctx, zmqpp::socket_type::pull);
    pull_->bind("inproc://MODULE.WEBSOCKET.INTERNAL_PULL");

    router_ = std::make_unique<zmqpp::socket>(ctx, zmqpp::socket_type::router);
    router_->bind("inproc://SERVICE.WEBSOCKET");
    reactor_.add(*router_, std::bind(&WebSockAPIModule::handle_router, this));
    reactor_.add(*pull_, std::bind(&WebSockAPIModule::handle_pull, this));
}

void WebSockAPIModule::run()
{
    wssrv_ = std::make_unique<WSServer>(*this, core_utils()->database());
    std::thread thread(std::bind(&WSServer::run, wssrv_.get(), interface_, port_));

    while (is_running_)
    {
        reactor_.poll();
    }
    INFO("Websocket module registered shutdown. Will now stop websocket server.");
    wssrv_->start_shutdown();
    thread.join();
}

CoreUtilsPtr WebSockAPIModule::core_utils()
{
    return utils_;
}

void WebSockAPIModule::handle_router()
{
    std::string client_identifier; // client here is an ZeroMQ generated ID for the
                                   // module that sent the request.
    std::string cmd;
    zmqpp::message msg;
    router_->receive(msg);
    ASSERT_LOG(msg.parts() >= 2, "Message doesn't contain enough frames.");
    msg >> client_identifier >> cmd;

    if (cmd == "REGISTER_HANDLER")
    {
        ASSERT_LOG(msg.remaining() == 1, "Message is ill formed.");
        std::string handler;
        msg >> handler;
        wssrv_->register_external_handler(handler, client_identifier);
    }
    else if (cmd == "SEND_MESSAGE")
    {
        ASSERT_LOG(msg.remaining() == 2, "message is ill formed.");
        std::string connection_identifier;
        std::string content;
        msg >> connection_identifier >> content;
        wssrv_->send_external_message(connection_identifier, content);
    }
    else if (cmd == "REGISTER_MODULE")
    {
        ASSERT_LOG(msg.remaining() == 1, "Message is ill formed.");
        std::string module_name;
        msg >> module_name;
        ASSERT_LOG(module_name_client_id_.count(module_name) == 0,
                   BUILD_STR("A module named " << module_name
                                               << " is already registered."));
        module_name_client_id_[module_name] = client_identifier;
    }
}

void WebSockAPIModule::handle_pull()
{
    // Forward message from the websocketpp thread to the
    // router.
    zmqpp::message msg;
    pull_->receive(msg);

    std::string first_frame;
    msg.get(first_frame, 0);

    if (first_frame == "SEND_TO_MODULE")
    {
        // Modify the message object so that it can be sent over the router socket.
        std::string module_name;
        msg.get(module_name, 1);
        ASSERT_LOG(
            module_name_client_id_.count(module_name),
            BUILD_STR("Module named " << module_name << " is not registered."));

        // We pop the "SEND_TO_MODULE" and "${MODULE_NAME}" frames...
        msg.pop_front();
        msg.pop_front();
        // ... and replace them with the ZMQ router client identifier for this
        // module.
        msg.push_front(module_name_client_id_[module_name]);
    }

    ASSERT_LOG(msg.parts() >= 2, "Ill formed message from WSServer");
    bool ret = router_->send(msg, true);
    ASSERT_LOG(ret, "Internal send would have blocked.");
}
