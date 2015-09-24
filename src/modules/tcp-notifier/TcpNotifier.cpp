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

#include <core/auth/Auth.hpp>
#include <tools/PropertyTreeExtractor.hpp>
#include "core/auth/WiegandCard.hpp"
#include "TcpNotifier.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::TCPNotifier;

TCPNotifierModule::TCPNotifierModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                                     const boost::property_tree::ptree &cfg,
                                     CoreUtilsPtr utils)
    : BaseModule(ctx, pipe, cfg, utils)
{
  process_config();
}

TCPNotifierModule::~TCPNotifierModule()
{
}

void enforce_xml_node_name(const std::string &expected,
                           const std::string &current)
{
  if (current != expected)
  {
    std::stringstream ss;
    ss << "Invalid configuration file content. Expected xml tag "
       << Colorize::green(expected) << " but has " << Colorize::green(current)
       << " instead.";
    throw ConfigException("", ss.str());
  }
}

void TCPNotifierModule::process_config()
{
  for (auto &&itr : config_.get_child("module_config"))
  {
    enforce_xml_node_name("instance", itr.first);
    std::vector<std::string> auth_sources;
    std::vector<std::string> connects;
    std::vector<std::string> binds;

    for (auto &&srcs : itr.second.get_child("sources"))
    {
      enforce_xml_node_name("source", srcs.first);
      auto name = srcs.second.get<std::string>("");
      auth_sources.push_back(name);
    }

    if (itr.second.get_child_optional("connect"))
    {
      for (auto &&srcs : itr.second.get_child("connect"))
      {
        enforce_xml_node_name("endpoint", srcs.first);
        auto name = srcs.second.get<std::string>("");
        connects.push_back(name);
      }
    }

    if (itr.second.get_child_optional("bind"))
    {
      for (auto &&srcs : itr.second.get_child("bind"))
      {
        enforce_xml_node_name("endpoint", srcs.first);
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
