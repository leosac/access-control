/*
    Copyright (C) 2014-2025 Leosac

    This file is part of Leosac Access Control.

    Leosac Access Control is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leosac Access Control is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "WSHelperThread.hpp"
#include "CRUDHandler.hpp"

namespace Leosac {
namespace Module {
namespace Libgpiod {

void WSHelperThread::register_ws_handlers(WebSockAPI::Service &ws_service) {
    ws_service.register_crud_handler("libgpiod.gpio", &CRUDHandler::instanciate);
}

void WSHelperThread::unregister_ws_handlers(WebSockAPI::Service &ws_service) {
    ws_service.unregister_handler("libgpiod.gpio.create");
    ws_service.unregister_handler("libgpiod.gpio.read");
    ws_service.unregister_handler("libgpiod.gpio.update");
    ws_service.unregister_handler("libgpiod.gpio.delete");
}

}
}
}
