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

#include <memory>
#include <json.hpp>

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{

class WSServer;

/**
 * This is the application-level object that provide
 * the API.
 *
 * One objected is instantiated per websocket client.
 */
class API {
      public:
        using json = nlohmann::json;

        API(WSServer &server);

        /**
         * Retrieve the current version number of Leosac.
         */
        json get_leosac_version() const;

        /**
         * Generate an authentication token.
         *
         * The request is excepted to contain credential.
         */
        json get_auth_token(const json &req);

      private:
        /**
         * The API server.
         */
        WSServer &server_;
};

using APIPtr = std::shared_ptr<API>;

}
}
}
