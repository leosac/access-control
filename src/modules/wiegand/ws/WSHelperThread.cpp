/*
    Copyright (C) 2014-2017 Leosac

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

#include "WSHelperThread.hpp"
#include "CRUDHandler.hpp"
#include "LeosacFwd.hpp"
#include "modules/websock-api/Service.hpp"

namespace Leosac
{
namespace Module
{
namespace Wiegand
{

WSHelperThread::WSHelperThread(const CoreUtilsPtr &core_utils)
    : BaseModuleSupportThread(core_utils, 0)
{
}

void WSHelperThread::unregister_ws_handlers(WebSockAPI::Service &ws_service)
{
    ws_service.unregister_handler("wiegand-reader.create");
    ws_service.unregister_handler("wiegand-reader.read");
    ws_service.unregister_handler("wiegand-reader.update");
    ws_service.unregister_handler("wiegand-reader.delete");
}

void WSHelperThread::register_ws_handlers(WebSockAPI::Service &ws_service)
{
    ws_service.register_crud_handler("wiegand-reader", &CRUDHandler::instanciate);
}
}
}
}
