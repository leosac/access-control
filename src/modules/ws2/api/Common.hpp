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

#include "modules/ws2/MyWSServer.hpp"
#include "modules/ws2/WS2Fwd.hpp"
#include "modules/ws2/api/Common.hpp"
#include "tools/leosac.hpp"

namespace Leosac
{
namespace Module
{
namespace WS2
{
namespace API
{
boost::optional<json> get_leosac_version(const json & /*in*/, ReqCtx /*ec*/);
boost::optional<json> get_leosac_version_coro(const json &, ReqCtx,
                                              boost::asio::yield_context);
}
}
}
}
