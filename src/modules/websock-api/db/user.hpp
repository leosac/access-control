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

#include <string>
#include <map>
#include <vector>
#include <odb/core.hxx>

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
namespace DB
{
/**
 * A user of the Websocket API.
 *
 * Instance of this class are persisted in database.
 */
#pragma db object
class User
{
      public:
        User(const std::string& username,
              const std::string& password);
        User() = default;

        const std::string &username() const;
        const std::string &password() const;

      private:
        friend class odb::access;

#pragma db id auto
        unsigned long id_;

#pragma db unique
#pragma db not_null
        std::string username_;

#pragma db not_null
        std::string password_;
};
}

}
}
}
