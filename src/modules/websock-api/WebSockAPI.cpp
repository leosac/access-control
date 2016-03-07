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

#include "WebSockAPI.hpp"
#include "WSServer.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

WebSockAPIModule::WebSockAPIModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                                               const boost::property_tree::ptree &cfg,
                                               CoreUtilsPtr utils) :
    BaseModule(ctx, pipe, cfg, utils)
{
    port_ = cfg.get<uint16_t>("module_config.port", 8976);
}

void WebSockAPIModule::run()
{
    WSServer srv;
    std::thread thread(std::bind(&WSServer::run, &srv, port_));

    while (is_running_)
    {
        reactor_.poll();
    }

    srv.start_shutdown();
    thread.join();
}

void APIAuth::invalidate_token(const std::string &token)
{
    auto itr = tokens_.find(token);
    if (itr != tokens_.end())
        tokens_.erase(itr);
}
