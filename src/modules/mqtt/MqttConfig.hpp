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

#define ODB_NO_BASE_VERSION
#include "hardware/ExternalServer.hpp"
#include "hardware/ExternalMessage.hpp"
#include "hardware/HardwareFwd.hpp"
#include "modules/mqtt/MqttFwd.hpp"
#include "tools/db/database.hpp"
#include <chrono>

#pragma db model version(1, 1, open)

namespace Leosac
{
namespace Module
{
namespace Mqtt
{

/**
 * An instance of this class represents the configuration
 * of one Mqtt server.
 */
#pragma db object callback(validation_callback) table("HARDWARE_ExternalServer_Mqtt")
struct MqttServerConfig : public Hardware::ExternalServer
{
    MqttServerConfig()
        : Hardware::ExternalServer("localhost", 1883)
        , client_id_("leosac")
        , subscribe_prefix_("homeassistant/")
        , publish_prefix_("leosac/")
        , ssl_(false)
      {
      }

    MqttServerConfig(const MqttServerConfig &) = default;

    std::string client_id() const
    {
        return client_id_;
    }

    std::string subscribe_prefix() const
    {
        return subscribe_prefix_;
    }

    std::string publish_prefix() const
    {
        return publish_prefix_;
    }

    std::string username() const
    {
        return username_;
    }

    std::string password() const
    {
        return password_;
    }

    bool ssl() const
    {
        return ssl_;
    }

    std::string ssl_ca_certs() const
    {
        return ssl_ca_certs_;
    }

    std::string ssl_client_certfile() const
    {
        return ssl_client_certfile_;
    }

    std::string ssl_client_keyfile() const
    {
        return ssl_client_keyfile_;
    }

    bool ssl_insecure() const
    {
        return ssl_insecure_;
    }

    /**
     * ODB callback wrt database operation.
     */
    void validation_callback(odb::callback_event, odb::database &) const;

    std::string client_id_;
    std::string subscribe_prefix_;
    std::string publish_prefix_;
    std::string username_;
    std::string password_;
    bool ssl_;
    std::string ssl_ca_certs_;
    std::string ssl_client_certfile_;
    std::string ssl_client_keyfile_;
    bool ssl_insecure_;
};

#pragma db object callback(validation_callback) table("HARDWARE_ExternalMessage_Mqtt")
struct MqttExternalMessage : public Hardware::ExternalMessage
{
    MqttExternalMessage()
    {
    }

    MqttExternalMessage(const MqttExternalMessage &) = default;
};

/**
 * Transient configuration object that stores the list of
 * mqtt servers use/configure.
 *
 * This object is either populated from the database or from
 * the XML configuration.
 **/
class MqttConfig
{
  public:
    MqttConfig() = default;

    void add_server(std::shared_ptr<const MqttServerConfig>);
    void add_topic(std::shared_ptr<const MqttExternalMessage>);
    const std::vector<std::shared_ptr<const MqttServerConfig>> &servers() const;
    const std::vector<std::shared_ptr<const MqttExternalMessage>> &topics() const;

  private:
    std::vector<std::shared_ptr<const MqttServerConfig>> servers_;
    std::vector<std::shared_ptr<const MqttExternalMessage>> topics_;
};
}
}
}
