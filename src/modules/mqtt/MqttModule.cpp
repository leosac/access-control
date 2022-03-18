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

#include "modules/mqtt/MqttModule.hpp"
#include "core/Scheduler.hpp"
#include "core/kernel.hpp"
#include "hardware/HardwareFwd.hpp"
#include "hardware/HardwareService.hpp"
#include "hardware/ExternalMessage.hpp"
#include "modules/mqtt/MqttConfig_odb.h"
#include "tools/log.hpp"
#include <boost/property_tree/ptree.hpp>
#include <memory>
#include <zmqpp/context.hpp>
#include <zmqpp/message.hpp>

using namespace Leosac::Hardware;
using namespace Leosac::Module::Mqtt;

MqttModule::MqttModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                                         boost::property_tree::ptree const &cfg,
                                         CoreUtilsPtr utils)
    : BaseModule(ctx, pipe, cfg, utils)
{
  process_config();

  for (auto &server : servers_)
  {
    server.register_sockets(&reactor_);
  }
}

MqttModule::~MqttModule()
{
}

void MqttModule::process_config()
{
  boost::property_tree::ptree module_config = config_.get_child("module_config");

  if (module_config.get<bool>("use_database", false))
  {
    load_db_config();
  }
  else
  {
    load_xml_config(module_config);
  }

  // Now we process the configuration object.
  for (const auto &server_config : mqtt_config_->servers())
  {
    using namespace Colorize;
    INFO("Creating Mqtt Server: "
         << green(underline(server_config->name()))
         << "\n\t Host: " << green(underline(server_config->host()))
         << "\n\t Port: " << green(underline(server_config->port()))
         << "\n\t Ssl: " << green(underline(server_config->ssl()))
         << "\n\t Username: " << green(underline(server_config->username())));

    MqttExternalServer server(ctx_, server_config, mqtt_config_->topics());
    utils_->config_checker().register_object(server.name(),
                                             Leosac::Hardware::DeviceClass::EXTERNAL_SERVER);
    servers_.push_back(std::move(server));
  }
}

void MqttModule::run()
{
    for (auto &server : servers_)
    {
        server.connect();
    }
    BaseModule::run();
    for (auto &server : servers_)
    {
        server.disconnect();
    }
}

void MqttModule::load_db_config()
{
    using namespace odb;
    using namespace odb::core;
    auto db = utils_->database();

    // First we load or update database schema if needed.
    schema_version v = db->schema_version("module_mqtt");
    schema_version cv(schema_catalog::current_version(*db, "module_mqtt"));
    if (v == 0)
    {
        transaction t(db->begin());
        INFO("Attempt to create module_mqtt SQL schema.");
        schema_catalog::create_schema(*db, "module_mqtt");
        t.commit();
    }
    else if (v < cv)
    {
        INFO("Mqtt Module performing database migration. Going from version "
             << v << " to version " << cv);
        transaction t(db->begin());
        schema_catalog::migrate(*db, cv, "module_mqtt");
        t.commit();
    }

    // Create empty configuration object...
    mqtt_config_ = std::make_unique<MqttConfig>();

    // ... then loads servers from database.
    odb::transaction t(utils_->database()->begin());
    odb::result<MqttServerConfig> sresult(
        utils_->database()->query<MqttServerConfig>());
    for (const auto &server : sresult)
    {
        MqttServerConfigPtr server_ptr =
            db->load<MqttServerConfig>(server.id());
        ASSERT_LOG(server_ptr, "Loading from database/cache failed");
        if (server_ptr->enabled())
            mqtt_config_->add_server(server_ptr);
    }
    t.commit();

    // ... and topics
    odb::transaction t2(utils_->database()->begin());
    odb::result<MqttExternalMessage> tresult(
        utils_->database()->query<MqttExternalMessage>());
    for (const auto &topic : tresult)
    {
        std::shared_ptr<MqttExternalMessage> topic_ptr =
            db->load<MqttExternalMessage>(topic.id());
        ASSERT_LOG(topic_ptr == nullptr, "Loading from database/cache failed");
        if (topic_ptr->enabled())
            mqtt_config_->add_topic(topic_ptr);
    }
    t2.commit();
    INFO("Mqtt module using SQL database for configuration.");
}

void MqttModule::load_xml_config(
    const boost::property_tree::ptree &module_config)
{
    mqtt_config_ = std::make_unique<MqttConfig>();
    for (auto &node : module_config.get_child("servers"))
    {
        auto server_config = std::make_shared<MqttServerConfig>();
        boost::property_tree::ptree xml_server_cfg = node.second;

        server_config->name(xml_server_cfg.get_child("name").data());
        server_config->host(xml_server_cfg.get<std::string>("host", "localhost"));
        server_config->port(xml_server_cfg.get<uint16_t>("port", 1883));
        server_config->client_id_ = xml_server_cfg.get<std::string>("client_id", "leosac");
        server_config->subscribe_prefix_ = xml_server_cfg.get<std::string>("subscribe_prefix", "homeassistant/");
        server_config->publish_prefix_ = xml_server_cfg.get<std::string>("publish_prefix", "leosac/");
        server_config->username_ = xml_server_cfg.get<std::string>("username", "");
        server_config->password_ = xml_server_cfg.get<std::string>("password", "");
        server_config->ssl_ = xml_server_cfg.get<bool>("ssl", false);
        server_config->ssl_ca_certs_ = xml_server_cfg.get<std::string>("ssl_ca_certs", "");
        server_config->ssl_client_certfile_ = xml_server_cfg.get<std::string>("ssl_client_certfile", "");
        server_config->ssl_client_keyfile_ = xml_server_cfg.get<std::string>("ssl_client_keyfile", "");
        server_config->ssl_insecure_ = xml_server_cfg.get<bool>("ssl_insecure", false);

        mqtt_config_->add_server(server_config);
    }

    for (const auto &node : module_config.get_child("topics"))
    {
        auto topic_config = std::make_shared<MqttExternalMessage>();
        boost::property_tree::ptree xml_topic_cfg = node.second;

        std::string direction;
        std::string virtualtype;

        topic_config->name(xml_topic_cfg.get_child("name").data());
        topic_config->subject(xml_topic_cfg.get_child("subject").data());
        direction = xml_topic_cfg.get_child("direction").data();
        topic_config->direction((direction == "subscribe") ? Leosac::Hardware::ExternalMessage::Direction::Subscribe : Leosac::Hardware::ExternalMessage::Direction::Publish);
        virtualtype = xml_topic_cfg.get_child("virtualtype").data();
        if (virtualtype == "rfidreader")
        {
          topic_config->virtualtype(Leosac::Hardware::DeviceClass::RFID_READER);
        }
        else if (virtualtype == "led")
        {
          topic_config->virtualtype(Leosac::Hardware::DeviceClass::LED);
        }
        else if (virtualtype == "buzzer")
        {
          topic_config->virtualtype(Leosac::Hardware::DeviceClass::BUZZER);
        }
        else
        {
          topic_config->virtualtype(Leosac::Hardware::DeviceClass::GPIO);
        }
        topic_config->payload(xml_topic_cfg.get<std::string>("payload", ""));

        mqtt_config_->add_topic(topic_config);
    }
}
