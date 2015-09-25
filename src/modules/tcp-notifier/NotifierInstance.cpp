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

#include <zmqpp/message.hpp>
#include <tools/log.hpp>
#include <core/auth/Auth.hpp>
#include <tools/Colorize.hpp>
#include "NotifierInstance.hpp"
#include "core/auth/WiegandCard.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::TCPNotifier;

NotifierInstance::NotifierInstance(zmqpp::context &ctx, zmqpp::reactor &reactor,
                                   std::vector<std::string> auth_sources,
                                   std::vector<std::string> connect_to,
                                   std::vector<std::string> bind_to,
                                   ProtocolHandlerUPtr protocol_handler)
    : bus_sub_(ctx, zmqpp::socket_type::sub)
    , tcp_(ctx, zmqpp::socket_type::stream)
    , protocol_(std::move(protocol_handler))
{
  ASSERT_LOG(connect_to.empty() || bind_to.empty(),
             "Cannot bind and connect at the same time.");
  ASSERT_LOG(!connect_to.empty() || !bind_to.empty(), "Cannot do nothing");
  ASSERT_LOG(protocol_, "No protocol handler");

  act_as_server_ = !bind_to.empty();

  bus_sub_.connect("inproc://zmq-bus-pub");
  for (const auto &src : auth_sources)
  {
    bus_sub_.subscribe("S_" + src);
  }
  reactor.add(bus_sub_, std::bind(&NotifierInstance::handle_msg_bus, this));

  configure_tcp_socket(act_as_server_ ? bind_to : connect_to);
  reactor.add(tcp_, std::bind(&NotifierInstance::handle_tcp_msg, this));
}

void NotifierInstance::handle_credential(Leosac::Auth::WiegandCard &card)
{
  for (const auto &target : targets_)
  {
    // Skip disconnected client.
    if (!target.status_)
      continue;
    zmqpp::message msg;

    msg << target.zmq_identity_;
    try
    {
      auto data = protocol_->build_cred_msg(card);
      msg.add_raw(&data[0], data.size());
    }
    catch (const ProtocolException &e)
    {
      WARN("TCPNotifier: Protocol error: " << e.what());
      continue;
    }
    auto ret = tcp_.send(msg, true);
    if (ret == false) // would block. woops
    {
      ERROR("Sending to client would block.");
    }
  }
}

void NotifierInstance::handle_one(zmqpp::message &msg)
{
  std::string routing_id;
  std::string data;

  assert(msg.parts() == 2);
  msg >> routing_id >> data;

  if (data.size() == 0)
  {
    auto target = find_target(routing_id);
    if (act_as_server_)
    {
      // As a server we drop disconnected peer, or create a TargetInfo
      // for newly connected peer.
      if (!target)
      {
        TargetInfo ti;
        ti.status_       = true;
        ti.zmq_identity_ = routing_id;

        targets_.push_back(std::move(ti));
        INFO("TCP-Notifier: New client connected.");
      }
      else
      {
        targets_.erase(std::remove_if(targets_.begin(), targets_.end(),
                                      [&](const TargetInfo &info)
                                      {
                                        return info.zmq_identity_ == routing_id;
                                      }),
                       targets_.end());
        INFO("TCP-Notifier: Client disconnected.");
      }
      return;
    }
    ASSERT_LOG(target, "Why did we lose a target?");

    if (target->status_)
      INFO("Lost connection with client.");
    else
      INFO("Successfully connected to client.");

    target->status_ = !target->status_;
  }
}

void NotifierInstance::handle_tcp_msg()
{
  zmqpp::message msg;

  while (tcp_.receive(msg, true))
  {
    handle_one(msg);
  }
}

NotifierInstance::TargetInfo *
NotifierInstance::find_target(const std::string &routing_id)
{
  auto itr = std::find_if(targets_.begin(), targets_.end(),
                          [&](const TargetInfo &target)
                          {
                            return target.zmq_identity_ == routing_id;
                          });
  if (itr != targets_.end())
    return &(*itr);
  return nullptr;
}

void NotifierInstance::handle_msg_bus()
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
    INFO("TCP-Notifier cannot handle this type of credential yet.");
    return;
  }
  auto wiegand_card = Auth::WiegandCard(card, bits);

  handle_credential(wiegand_card);
}

void NotifierInstance::configure_tcp_socket(
    const std::vector<std::string> &endpoints)
{
  tcp_.set(zmqpp::socket_option::backlog, 5);
  if (act_as_server_)
  {
    for (auto &endpoint : endpoints)
    {
      INFO("TCP-Notifier binding to: tcp://" << endpoint);
      tcp_.bind("tcp://" + endpoint);
    }
  }
  else
  {
    for (auto &endpoint : endpoints)
    {
      TargetInfo target;
      target.url_    = "tcp://" + endpoint;
      target.status_ = false;
      tcp_.connect(target.url_);
      INFO("TCP-Notifier remote target: " << Colorize::green(target.url_));
      tcp_.get(zmqpp::socket_option::identity, target.zmq_identity_);
      targets_.push_back(std::move(target));
    }
  }
}
