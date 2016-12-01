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

#include "modules/websock-api/Service.hpp"
#include "modules/websock-api/WSServer.hpp"

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{

bool Service::register_typed_handler(const Service::WSHandler &handler,
                                     const std::string &type)
{
    return server_.register_asio_handler(handler, type);
}
}
}
}
