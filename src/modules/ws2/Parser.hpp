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

#include "LeosacFwd.hpp"
#include "core/APIStatusCode.hpp"
#include "core/auth/AuthFwd.hpp"
#include "modules/ws2/WS2Fwd.hpp"
#include <json.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

namespace Leosac
{
namespace Module
{
namespace WS2
{

struct ClientMessage
{
    std::string uuid;
    std::string type;
    nlohmann::json content;
};


/**
 * A message sent by the server to a client.
 */
struct ServerMessage
{
    /**
     * Build a partial ServerMessage from a ClientMessage.
     *
     * This simply copies the UUID and type from the ClientMessage
     * into the ServerMessage.
     */
    ServerMessage(const ClientMessage &from)
        : status_code(APIStatusCode::UNKNOWN)
        , uuid(from.uuid)
        , type(from.type)
    {
    }

    ServerMessage()
        : status_code(APIStatusCode::UNKNOWN)
    {
    }
    APIStatusCode status_code;
    std::string status_string;
    std::string uuid;
    std::string type;
    json content;
};


class Parser
{
    using MessagePtr = websocketpp::server<websocketpp::config::asio>::message_ptr;
    using json       = nlohmann::json;

  public:
    ClientMessage parse(const MessagePtr &msg);
};
}
}
}