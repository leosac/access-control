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

#include "modules/wiegand/WSHelperThread.hpp"
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
}

void WSHelperThread::register_ws_handlers(WebSockAPI::Service &ws_service)
{
}
}
}
}
