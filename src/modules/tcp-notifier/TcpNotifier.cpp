/*
    Copyright (C) 2014-2016 Islog

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

#include "TcpNotifier.hpp"
#include "core/auth/WiegandCard.hpp"
#include <core/auth/Auth.hpp>
#include <tools/PropertyTreeExtractor.hpp>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::TCPNotifier;

TCPNotifierModule::TCPNotifierModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                                     const boost::property_tree::ptree &cfg,
                                     CoreUtilsPtr utils)
    : BaseModule(ctx, pipe, cfg, utils)
    , xmlnne_("") // fixme maybe: we don't have access to kernel config path at this
                  // point.
{
    process_config();
}

TCPNotifierModule::~TCPNotifierModule()
{
}

void TCPNotifierModule::process_config()
{
    for (auto &&itr : config_.get_child("module_config"))
    {
        xmlnne_("instance", itr.first);
        std::vector<std::string> auth_sources;
        std::vector<std::string> connects;
        std::vector<std::string> binds;

        for (auto &&srcs : itr.second.get_child("sources"))
        {
            xmlnne_("source", srcs.first);
            auto name = srcs.second.get<std::string>("");
            auth_sources.push_back(name);
        }

        if (itr.second.get_child_optional("connect"))
        {
            for (auto &&srcs : itr.second.get_child("connect"))
            {
                xmlnne_("endpoint", srcs.first);
                auto name = srcs.second.get<std::string>("");
                connects.push_back(name);
            }
        }

        if (itr.second.get_child_optional("bind"))
        {
            for (auto &&srcs : itr.second.get_child("bind"))
            {
                xmlnne_("endpoint", srcs.first);
                auto name = srcs.second.get<std::string>("");
                binds.push_back(name);
            }
        }

        if (!(connects.empty() || binds.empty()))
        {
            ERROR("Bind or connect. Not both !");
            continue;
        }

        Tools::PropertyTreeExtractor extractor(itr.second, "TCP-Notifier");
        int protocol_id = extractor.get<int>("protocol");
        auto protocol   = ProtocolHandler::create(protocol_id);

        if (!protocol)
        {
            ERROR("Cannot instanciate a protocol number " << protocol_id);
            continue;
        }
        auto ni = std::make_unique<NotifierInstance>(
            ctx_, reactor_, auth_sources, connects, binds, std::move(protocol));
        instances_.push_back(std::move(ni));
    }
}
