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

#include "NotifdModule.hpp"
#include "core/GetServiceRegistry.hpp"
#include <boost/asio.hpp>

namespace Leosac
{

namespace Module
{
namespace Notifd
{
NotifdModule::NotifdModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                           const boost::property_tree::ptree &cfg,
                           CoreUtilsPtr utils)
    : AsioModule(ctx, pipe, cfg, utils)
{
}

NotifdModule::~NotifdModule()
{
    ASSERT_LOG(io_service_.stopped(), "io_service not stopped.");
}
}
}
}
