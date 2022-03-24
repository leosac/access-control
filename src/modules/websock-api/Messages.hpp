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

#include "core/APIStatusCode.hpp"
#include "core/audit/AuditFwd.hpp"
#include <nlohmann/json.hpp>
#include <string>

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
using json = nlohmann::json;

/**
 * A message sent by the server to a client.
 */
struct ServerMessage
{
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

/**
 * A message sent by a client to Leosac.
 */
struct ClientMessage
{
    std::string uuid;
    std::string type;
    json content;
};

/**
 * This is an appropriate representation of a incoming WS
 * message as seen by a module.
 */
struct ModuleIncomingMessage
{
    ModuleIncomingMessage()
        : audit_id(0)
    {
    }
    /**
     * The AuditEntry id that traces the request.
     */
    Audit::AuditEntryId audit_id;

    /**
     * An identifier that let the WebsSock find the actual connection.
     */
    std::string connection_identifier;

    /**
     * The content of message.
     */
    std::string content;
};
}
}
}
