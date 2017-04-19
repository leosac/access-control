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

#include "WS2.hpp"
#include "core/CoreAPI.hpp"
#include "core/CoreUtils.hpp"
#include "modules/ws2/MyWSServer.hpp"
#include "tools/XmlPropertyTree.hpp"
#include <boost/asio/spawn.hpp>
#include <boost/filesystem.hpp>
#include <zmqpp/proxy.hpp>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WS2;

WS2Module::WS2Module(zmqpp::context &ctx, zmqpp::socket *pipe,
                     const boost::property_tree::ptree &cfg, CoreUtilsPtr utils)
    : AsioModule(ctx, pipe, cfg, utils)
{
    port_      = cfg.get<uint16_t>("module_config.port", 8976);
    interface_ = cfg.get<std::string>("module_config.interface", "127.0.0.1");

    auto endpoint_colorized = Colorize::green(
        Colorize::underline(fmt::format("{}:{}", interface_, port_)));
    INFO(Colorize::green("WS2") << " module binding to " << endpoint_colorized);
}

WS2Module::~WS2Module()
{
}

CoreUtilsPtr WS2Module::core_utils()
{
    return utils_;
}

void WS2Module::on_startup()
{
    DEBUG("WS2Module will instanciate MyWSServer");
    server_ = std::make_unique<MyWSServer>(interface_, port_, utils_);
}
