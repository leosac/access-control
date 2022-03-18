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

#pragma once

#include "modules/mqtt/MqttConfig.hpp"
#include "hardware/facades/FGPIO.hpp"
#include "tools/log.hpp"
#include <chrono>
#include <zmqpp/zmqpp.hpp>
#include <mqtt/async_client.h>

namespace Leosac
{
namespace Module
{
namespace Mqtt
{
/**
* Implementation class, for use by the Mqtt module only.
*/
class MqttExternalServer
{
  public:
    /**
    * @param ctx ZMQ context
    * @param config configuration of the server.
    */
    MqttExternalServer(zmqpp::context &ctx, std::shared_ptr<const MqttServerConfig> config,
      const std::vector<std::shared_ptr<const MqttExternalMessage>> &topics);

    void register_sockets(zmqpp::reactor *reactor);

    void connect();

    void disconnect();

    /**
    * Request received on the topic specific socket.
    */
    void handle_topic_request(const std::string& topic_name);

    /**
    * Message received from mqtt.
    */
    void handle_mqtt_msg(mqtt::const_message_ptr msg);

    /**
    * Someone sent a request.
    */
    void handle_request();

    std::shared_ptr<const MqttServerConfig> config() const;

    const std::vector<std::shared_ptr<const MqttExternalMessage>>& topics() const;

    const std::string& name() const;

  private:

    /**
    * REP socket to receive command on.
    */
    zmqpp::socket sock_;

    /**
    * Socket to write to the message bus.
    */
    zmqpp::socket bus_push_;

    /**
    * REP socket to receive command on for topics.
    */
    std::map<std::string, zmqpp::socket> topic_socks_;

    /**
    * The MQTT client.
    */
    std::shared_ptr<mqtt::async_client> client_;

    std::shared_ptr<const MqttServerConfig> config_;

    std::vector<std::shared_ptr<const MqttExternalMessage>> topics_;
};
}
}
}
