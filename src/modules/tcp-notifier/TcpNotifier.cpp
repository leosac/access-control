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
#include "core/auth/WiegandCard.hpp"
#include "TcpNotifier.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::TCPNotifier;

TCPNotifierModule::TCPNotifierModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                                     const boost::property_tree::ptree &cfg,
                                     CoreUtilsPtr utils)
    : BaseModule(ctx, pipe, cfg, utils)
    , bus_sub_(ctx, zmqpp::socket_type::sub)
    , tcp_(ctx, zmqpp::socket_type::stream)
{
  bus_sub_.connect("inproc://zmq-bus-pub");
  reactor_.add(bus_sub_, std::bind(&TCPNotifierModule::handle_msg_bus, this));
  reactor_.add(tcp_, std::bind(&TCPNotifierModule::handle_tcp_msg, this));

  process_config();
}

TCPNotifierModule::~TCPNotifierModule()
{
}

void TCPNotifierModule::handle_msg_bus()
{
  zmqpp::message msg;
  std::string src;
  Leosac::Auth::SourceType type;
  std::string card;
  int bits;

  bus_sub_.receive(msg);
  if (msg.parts() < 4)
  {
    WARN("Unexpected message content.");
    return;
  }
  msg >> src >> type >> card >> bits;
  if (type != Leosac::Auth::SourceType::SIMPLE_WIEGAND)
  {
    INFO("WS-Notifier cannot handle this type of credential yet.");
    return;
  }

  send_card_info_to_remote(card, bits);
}

void TCPNotifierModule::process_config()
{
  for (auto &&itr : config_.get_child("module_config.sources"))
  {
    auto name = itr.second.get<std::string>("");
    bus_sub_.subscribe("S_" + name);
  }

  for (auto &&itr : config_.get_child("module_config.targets"))
  {
    TargetInfo target;
    target.url_    = itr.second.get<std::string>("url");
    target.status_ = false;

    INFO("TCP-Notifier remote target: " << Colorize::green(target.url_));

    tcp_.connect("tcp://" + target.url_);
    tcp_.get(zmqpp::socket_option::identity, target.zmq_identity_);
    INFO("ZMQ-Routing ID: {" << Colorize::green(target.zmq_identity_) << "}");

    targets_.push_back(std::move(target));
  }
}

void TCPNotifierModule::send_card_info_to_remote(const std::string &card_hex,
                                                 int nb_bits)
{
  auto card = Auth::WiegandCard(card_hex, nb_bits);
  for (const auto &target : targets_)
  {
    // Skip disconnected client.
    if (!target.status_)
      continue;
    zmqpp::message msg;

    msg << target.zmq_identity_;
    msg << card.to_int();
    auto ret = tcp_.send(msg, true);
    if (ret == false) // would block. woops
    {
      ERROR("Sending to client would block.");
    }
  }
}

void TCPNotifierModule::handle_tcp_msg()
{
  zmqpp::message msg;

  tcp_.receive(msg);
  std::string routing_id;
  std::string data;

  assert(msg.parts() == 2);
  msg >> routing_id >> data;

  INFO("Received TCP data from client " << routing_id << ", data {" << data
                                        << "}");
  if (data.size() == 0)
  {
    auto &target = find_target(routing_id);
    if (target.status_)
      INFO("Lost connection with client");
    else
      INFO("Successfully connected to client.");

    target.status_ = !target.status_;
  }
}

TCPNotifierModule::TargetInfo &
TCPNotifierModule::find_target(const std::string &routing_id)
{
  auto itr = std::find_if(targets_.begin(), targets_.end(),
                          [&](const TargetInfo &target)
                          {
                            return target.zmq_identity_ == routing_id;
                          });
  assert(itr != targets_.end());
  return *itr;
}
