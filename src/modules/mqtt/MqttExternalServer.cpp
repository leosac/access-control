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

#include "modules/mqtt/MqttExternalServer.hpp"
#include "tools/log.hpp"
#include <core/auth/Auth.hpp>
#include <nlohmann/json.hpp>
#include <iomanip>

using namespace Leosac::Module::Mqtt;
using namespace Leosac::Hardware;
using namespace Leosac::Auth;

const int QOS = 1;
const int N_RETRY_ATTEMPTS = 60;

class mqtt_callback : public virtual mqtt::callback,
  public virtual mqtt::iaction_listener
{
  // Counter for the number of connection retries
  int nretry_;
  // The MQTT client
  std::shared_ptr<mqtt::async_client> cli_;
  // Options to use if we need to reconnect
  mqtt::connect_options& connOpts_;
  MqttExternalServer& server_;

  void reconnect()
  {
    INFO("Will try to reconnect to MQTT broker in 10 seconds...");
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    try
    {
      cli_->connect(connOpts_, nullptr, *this);
    }
    catch (const mqtt::exception& e)
    {
      ERROR(e.what());
    }
  }

  void on_failure(const mqtt::token& tok) override
  {
    ERROR("MQTT broker connection attempt failed.");
    if (++nretry_ <= N_RETRY_ATTEMPTS)
    {
      reconnect();
    }
    else
    {
      ERROR("Maximum retry attempts reached. We will not try to reconnect to the MQTT broker anymore.");
    }
  }

  void on_success(const mqtt::token& tok) override
  {
  }

  void connected(const std::string& cause) override
  {
    INFO("Connected successfully to the MQTT broker.");

    for (const auto& topic : server_.topics())
    {
      cli_->subscribe(server_.config()->subscribe_prefix() + topic->subject(), QOS);
    }
  }

  void connection_lost(const std::string& cause) override
  {
    WARN("Connection lost to the MQTT broker.");
    if (!cause.empty())
    {
      INFO(cause);
    }

    WARN("Reconnecting to MQTT broker...");
    nretry_ = 0;
    reconnect();
  }

  void message_arrived(mqtt::const_message_ptr msg) override
  {
    INFO("MQTT message arrived:"
      << "\n\t Topic: " << msg->get_topic()
      << "\n\t Payload: " << msg->to_string());

    server_.handle_mqtt_msg(msg);
  }

  void delivery_complete(mqtt::delivery_token_ptr tok) override
  {
  }

public:
  mqtt_callback(std::shared_ptr<mqtt::async_client> cli, mqtt::connect_options& connOpts, MqttExternalServer& server)
    : nretry_(0), cli_(cli), connOpts_(connOpts), server_(server)
  {
  }
};

MqttExternalServer::MqttExternalServer(zmqpp::context &ctx,
                                     std::shared_ptr<const MqttServerConfig> config,
                                     const std::vector<std::shared_ptr<const MqttExternalMessage>> &topics)
    : sock_(ctx, zmqpp::socket_type::rep)
    , bus_push_(ctx, zmqpp::socket_type::push)
    , config_(config)
    , topics_(topics)
{
  bus_push_.connect("inproc://zmq-bus-pull");

  sock_.bind("inproc://" + config->name());

  for (const auto &topic : topics)
  {
    if (topic->direction() == Hardware::ExternalMessage::Direction::Publish)
    {
      auto topic_sock = zmqpp::socket(ctx, zmqpp::socket_type::rep);
      topic_sock.bind("inproc://" + topic->name());
      topic_socks_.insert(
        topic_socks_.end(),
        std::pair<std::string, zmqpp::socket>(
          topic->name(),
          std::move(topic_sock)
        )
      );
    }
  }
}

void MqttExternalServer::register_sockets(zmqpp::reactor *reactor)
{
  reactor->add(sock_,
               std::bind(&MqttExternalServer::handle_request, this));
  for (auto const& t : topic_socks_)
  {
    reactor->add(t.second,
                 std::bind(&MqttExternalServer::handle_topic_request, this, t.first));
  }
}

void MqttExternalServer::connect()
{
  client_ = std::make_shared<mqtt::async_client>(std::string(config_->ssl() ? "ssl" : "tcp") +
    "://" + config_->host() + ":" + std::to_string(config_->port()),
    config_->client_id());
  auto connOpts = mqtt::connect_options_builder()
    .clean_session()
    .finalize();
  if (!config_->username().empty())
  {
    connOpts.set_user_name(config_->username());
    connOpts.set_password(config_->password());
  }
  if (config_->ssl())
  {
    connOpts.set_ssl(mqtt::ssl_options(config_->ssl_ca_certs(),
      config_->ssl_client_certfile(),
      config_->ssl_client_keyfile(),
      "",
      "ALL",
      config_->ssl_insecure()));
  }

  mqtt_callback cb(client_, connOpts, *this);
  client_->set_callback(cb);

  client_->connect(connOpts, nullptr, cb);
}

void MqttExternalServer::disconnect()
{
  client_->disconnect();
}

void MqttExternalServer::handle_topic_request(const std::string& topic_name)
{
  zmqpp::message msg;
  auto& topic_sock = topic_socks_.at(topic_name);
  topic_sock.receive(msg);
  std::string topic_value;
  msg >> topic_value;

  mqtt::message_ptr pubmsg;
  std::shared_ptr<const MqttExternalMessage> extmsg;
  for (const auto topic : topics_)
  {
    if (topic->name() == topic_name)
    {
      extmsg = topic;
    }
  }
  ASSERT_LOG(extmsg != nullptr, "Unknown topic.");

  bool interupt = false;
  if (extmsg->virtualtype() == DeviceClass::GPIO || extmsg->virtualtype() == DeviceClass::LED || extmsg->virtualtype() == DeviceClass::BUZZER)
  {
    std::string state;
    msg >> state; // should be ON or OFF
    std::string payload = extmsg->payload();
    if (payload.empty())
    {
      payload = state;
    }
    else
    {
      payload = boost::replace_all_copy(payload, "__PLACEHOLDER__", state);
    }
    INFO("Payload to be published as MQTT message:" << payload);
    pubmsg = mqtt::make_message(config_->publish_prefix() + extmsg->name(), payload);
    interupt = true;
  }

  ASSERT_LOG(pubmsg != nullptr, "Unsupported topic virtual type for MQTT module.");
  pubmsg->set_qos(QOS);

  try
  {
    client_->publish(pubmsg);

    topic_sock.send("OK");

    if (interupt)
    {
      zmqpp::message zmsg;
      zmsg << ("S_INT:" + extmsg->name()) << topic_value; // Should be ON or OFF
      bus_push_.send(zmsg);
    }
  }
  catch(const mqtt::exception& e)
  {
    topic_sock.send("KO");
    ERROR(e.what());
  }
}

void MqttExternalServer::handle_mqtt_msg(mqtt::const_message_ptr msg)
{
  std::shared_ptr<const MqttExternalMessage> extmsg;
  for (const auto topic : topics_)
  {
    if (config_->subscribe_prefix() + topic->subject() == msg->get_topic())
    {
      extmsg = topic;
    }
  }
  ASSERT_LOG(extmsg != nullptr, "Unknown topic.");

  const std::string payload = extmsg->payload();
  std::string msg_value = msg->to_string();
  if (!payload.empty())
  {
    auto msg_json = nlohmann::json::parse(msg->to_string());
    auto d = msg_json.at(payload);
    if (!d.is_null())
    {
      msg_value = d.get<std::string>();
    }
    else
    {
      ERROR("Cannot found json key `" << payload << "` on received MQTT message.");
    }
  }
  INFO("Extracted value to be forwarded to ZMQ: " << msg_value);

  if (extmsg->virtualtype() == DeviceClass::GPIO || extmsg->virtualtype() == DeviceClass::LED || extmsg->virtualtype() == DeviceClass::BUZZER)
  {
    if (msg_value == "1")
    {
      msg_value = "ON";
    }
    else if (msg_value == "0")
    {
      msg_value = "OFF";
    }

    zmqpp::message zmsg;
    zmsg << ("S_INT:" + extmsg->name()) << msg_value; // Should be ON or OFF
    bus_push_.send(zmsg);
  }
  else if (extmsg->virtualtype() == DeviceClass::RFID_READER)
  {
    zmqpp::message zmsg;
    zmsg << ("S_" + extmsg->name()) << Leosac::Auth::SourceType::SIMPLE_CSN
        << msg_value;
    bus_push_.send(zmsg);
  }
}

void MqttExternalServer::handle_request()
{
  zmqpp::message msg;
  std::string str;
  sock_.receive(msg);

  msg >> str;
  assert(str == "CONNECT" || str == "DISCONNECT");
  if (str == "CONNECT")
  {
    msg.pop_front();
    try
    {
      connect();
      sock_.send("OK");
    }
    catch(const mqtt::exception& e)
    {
      ERROR(e.what());
      sock_.send("KO");
    }
  }
  else if (str == "DISCONNECT")
  {
    msg.pop_front();
    try
    {
      disconnect();
      sock_.send("OK");
    }
    catch(const mqtt::exception& e)
    {
      ERROR(e.what());
      sock_.send("KO");
    }
  }
}

const std::string& MqttExternalServer::name() const
{
    return config_->name();
}

const std::vector<std::shared_ptr<const MqttExternalMessage>>& MqttExternalServer::topics() const
{
  return topics_;
}

std::shared_ptr<const MqttServerConfig> MqttExternalServer::config() const
{
  return config_;
}
