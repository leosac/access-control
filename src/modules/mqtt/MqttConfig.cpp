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

#include "exception/ModelException.hpp"
#include "modules/mqtt/MqttConfig_odb.h"
#include <odb/pgsql/query.hxx>

namespace Leosac
{
namespace Module
{
namespace Mqtt
{

void MqttConfig::add_server(std::shared_ptr<const MqttServerConfig> server)
{
    servers_.push_back(server);
}

const std::vector<std::shared_ptr<const MqttServerConfig>> &MqttConfig::servers() const
{
    return servers_;
}

void MqttConfig::add_topic(std::shared_ptr<const MqttExternalMessage> topic)
{
    topics_.push_back(topic);
}

const std::vector<std::shared_ptr<const MqttExternalMessage>> &MqttConfig::topics() const
{
    return topics_;
}

void MqttServerConfig::validation_callback(odb::callback_event e,
                                              odb::database &db) const
{
    Device::validation_callback(e, db);
}
}
}
}
