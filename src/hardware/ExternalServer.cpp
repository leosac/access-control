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

#include "hardware/ExternalServer.hpp"

namespace Leosac
{
namespace Hardware
{

ExternalServer::ExternalServer()
    : Device(DeviceClass::EXTERNAL_SERVER)
{
}

ExternalServer::ExternalServer(const std::string& host, uint16_t port)
    : Device(DeviceClass::EXTERNAL_SERVER),
    host_(host),
    port_(port)
{
}

std::string ExternalServer::host() const
{
    return host_;
}

void ExternalServer::host(std::string host)
{
    host_ = host;
}

uint16_t ExternalServer::port() const
{
    return port_;
}

void ExternalServer::port(uint16_t port)
{
    port_ = port;
}

void ExternalServer::validation_callback(odb::callback_event e, odb::database &db) const
{
    Device::validation_callback(e, db);
}
}
}
