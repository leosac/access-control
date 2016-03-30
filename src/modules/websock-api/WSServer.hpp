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

#pragma once

#include <set>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include "api/api.hpp"
#include "tools/db/db_fwd.hpp"
#include "WebSockAPI.hpp"

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
class WSServer {
      public:

        /**
         * @param database A (non-null) pointer to the
         * database.
         */
        WSServer(WebSockAPIModule &module,
                 DBPtr database);

        using Server = websocketpp::server<websocketpp::config::asio>;
        using ConnectionAPIMap = std::map<websocketpp::connection_hdl,
            APIPtr,
            std::owner_less<websocketpp::connection_hdl>>;

        void run(uint16_t port);

        Server srv_;

        /**
         * Start the process of shutting down the server.
         *
         * The server will stop listening for new connection and will
         * attempt to close existing one.
         */
        void start_shutdown();

        /**
         * Retrieve the authentication helper.
         */
        APIAuth &auth();

        /**
         * Retrieve database handle
         */
        DBPtr db();

        /**
         * Retrieve the CoreUtils pointer.
         */
        CoreUtilsPtr core_utils();

      private:
        using json = nlohmann::json;

        void on_open(websocketpp::connection_hdl hdl);

        void on_close(websocketpp::connection_hdl hdl);

        void on_message(websocketpp::connection_hdl hdl, Server::message_ptr msg);

        /**
         * Process a request from a client.
         * The proper implementation method of WebSockAPI::API is called.
         */
        json dispatch_request(APIPtr api_handle, json &in);

        ConnectionAPIMap connection_api_;
        APIAuth auth_;

        /**
         * This maps (string) command name to API method.
         */
        std::map<std::string, json (API::*)(const json &)> handlers_;

        /**
         * Handler to the database.
         */
        DBPtr db_;

        /**
         * A reference to the module.
         *
         * The module is garanteed to outlive the WSServer.
         */
        WebSockAPIModule &module_;
};
}
}
}
