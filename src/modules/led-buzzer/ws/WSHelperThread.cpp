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
#include "hardware/serializers/BuzzerSerializer.hpp"
#include "hardware/serializers/LEDSerializer.hpp"
#include "modules/websock-api/Service.hpp"

namespace Leosac
{
namespace Module
{
namespace LedBuzzer
{

WSHelperThread::WSHelperThread(const CoreUtilsPtr &core_utils)
    : BaseModuleSupportThread(core_utils, 0)
{
}

void WSHelperThread::unregister_ws_handlers(WebSockAPI::Service &ws_service)
{
    ws_service.unregister_handler("buzzer.create");
    ws_service.unregister_handler("buzzer.read");
    ws_service.unregister_handler("buzzer.update");
    ws_service.unregister_handler("buzzer.delete");

    ws_service.unregister_handler("led.create");
    ws_service.unregister_handler("led.read");
    ws_service.unregister_handler("led.update");
    ws_service.unregister_handler("led.delete");
}

void WSHelperThread::register_ws_handlers(WebSockAPI::Service &ws_service)
{
    ws_service.register_crud_handler(
        "buzzer",
        &CRUDHandler<Hardware::Buzzer, CRUDHandlerHelper::buzzer_object_id_key,
                     Hardware::BuzzerSerializer>::instanciate);
    ws_service.register_crud_handler(
        "led", &CRUDHandler<Hardware::LED, CRUDHandlerHelper::led_object_id_key,
                            Hardware::LEDSerializer>::instanciate);
}
}
}
}
