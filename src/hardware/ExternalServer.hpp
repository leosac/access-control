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

#include "hardware/Device.hpp"
#include "hardware/HardwareFwd.hpp"
#include <odb/callback.hxx>
#include <odb/core.hxx>
#include <string>

namespace Leosac
{
namespace Hardware
{

/**
 *  Abstraction of an External Server device.
 *
 */
#pragma db object callback(validation_callback) table("HARDWARE_ExternalServer")
class ExternalServer : public Device
{
  public:

    ExternalServer();

    ExternalServer(const std::string& host, uint16_t port);

    void validation_callback(odb::callback_event, odb::database &) const override;

    std::string host() const;

    void host(const std::string host);

    uint16_t port() const;

    void port(uint16_t port);

  protected:
    std::string host_;
    uint16_t port_;

  private:
    friend odb::access;
};
}
}
