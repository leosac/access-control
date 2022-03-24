/*
    Copyright (C) 2014-2022 Leosac

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

#include "WebSockAPI.hpp"
#include "WSServer.hpp"
#include "core/CoreAPI.hpp"
#include "core/CoreUtils.hpp"
#include "tools/XmlPropertyTree.hpp"
#include <boost/filesystem.hpp>
#include <zmqpp/proxy.hpp>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

WebSockAPIModule::WebSockAPIModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                                   const boost::property_tree::ptree &cfg,
                                   CoreUtilsPtr utils)
    : BaseModule(ctx, pipe, cfg, utils)
{
    port_      = cfg.get<uint16_t>("module_config.port", 8976);
    interface_ = cfg.get<std::string>("module_config.interface", "127.0.0.1");

    auto endpoint_colorized = Colorize::green(
        Colorize::underline(fmt::format("{}:{}", interface_, port_)));
    INFO(Colorize::green("WEBSOCKET_API") << " module binding to "
                                          << endpoint_colorized);
}

void WebSockAPIModule::run()
{
    wssrv_ = std::make_unique<WSServer>(*this, core_utils()->database());
    std::thread thread(std::bind(&WSServer::run, wssrv_.get(), interface_, port_));

    while (is_running_)
    {
        reactor_.poll();
    }
    INFO("Websocket module registered shutdown. Will now stop websocket server.");
    wssrv_->start_shutdown();
    thread.join();
}

CoreUtilsPtr WebSockAPIModule::core_utils()
{
    return utils_;
}
