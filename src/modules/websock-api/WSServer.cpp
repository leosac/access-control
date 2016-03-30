/*
    Copyright (C) 2014-2015 Islog

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

#include <json.hpp>
#include "tools/log.hpp"
#include "WSServer.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

using json = nlohmann::json;

WSServer::WSServer(WebSockAPIModule &module, DBPtr database, DBPtr log_database) :
    auth_(*this),
    db_(database),
    log_db_(log_database),
    module_(module)
{
    assert(db_);
    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;
    srv_.init_asio();

    srv_.set_open_handler(std::bind(&WSServer::on_open, this, _1));
    srv_.set_close_handler(std::bind(&WSServer::on_close, this, _1));
    srv_.set_message_handler(std::bind(&WSServer::on_message, this, _1, _2));
    srv_.set_reuse_addr(true);

    handlers_["get_leosac_version"] = &API::get_leosac_version;
    handlers_["create_auth_token"] = &API::create_auth_token;
    handlers_["authenticate_with_token"] = &API::authenticate_with_token;
    handlers_["logout"] = &API::logout;
    handlers_["system_overview"] = &API::system_overview;
    handlers_["get_logs"] = &API::get_logs;
}

void WSServer::on_open(websocketpp::connection_hdl hdl)
{
    INFO("New WebSocket connection !");
    connection_api_.insert(std::make_pair(hdl,
                                          std::make_shared<API>(*this)));
}

void WSServer::on_close(websocketpp::connection_hdl hdl)
{
    INFO("WebSocket connection closed.");
    connection_api_.erase(hdl);
}

void WSServer::on_message(websocketpp::connection_hdl hdl, Server::message_ptr msg)
{
    json req = json::parse(msg->get_payload());

    assert(connection_api_.find(hdl) != connection_api_.end());
    auto api_handle = connection_api_.find(hdl)->second;

    INFO("Incoming payload: \n" << req.dump(4));

    try
    {
        json rep;

        rep["content"] = dispatch_request(api_handle, req);
        rep["uuid"] = req["uuid"];
        srv_.send(hdl, rep.dump(4), websocketpp::frame::opcode::text);
    }
    catch (const std::exception &e)
    {
        WARN("Exception when parsing request: " << e.what());
        return;
    }
}

void WSServer::run(uint16_t port)
{
    srv_.listen(port);
    srv_.start_accept();
    srv_.run();
}

void WSServer::start_shutdown()
{
    srv_.stop_listening();
    for (auto con_api : connection_api_)
    {
        srv_.close(con_api.first, 0, "bye");
    }
}

APIAuth &WSServer::auth()
{
    return auth_;
}

json WSServer::dispatch_request(APIPtr api_handle, json &in)
{
    auto command = in.at("cmd");
    auto handler_method = handlers_.find(command);
    json content;

    try
    {
        content = in.at("content");
    }
    catch (const std::exception &)
    {
        // ignore, as no content may be valid.
    }

    if (handler_method != handlers_.end())
    {
        auto method_ptr = handler_method->second;
        return ((*api_handle).*method_ptr)(content);
    }
    else
    {
        INFO("Ignore invalid WebSocketAPI command: " << command);
        return {};
    }
}

DBPtr WSServer::db()
{
    return db_;
}

CoreUtilsPtr WSServer::core_utils()
{
    return module_.core_utils();
}

DBPtr WSServer::log_db()
{
    return log_db_;
}
