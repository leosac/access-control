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

#include <string>
#include <zmqpp/zmqpp.hpp>

namespace Leosac
{
namespace Hardware
{

/**
* Facade object for a Wiegand Reader device.
*
* You can control a reader through this class by acting on its led and buzzer.
*
* @note This class implements the client code to [theses specifications](@ref
* hardware_spec_wiegand).
*/
class FExternalServer
{
  public:
    /**
    * Construct a facade to an external server; this facade will connect to the server
    * @param ctx ZMQ context
    * @param server_name name of targeted server
    */
    FExternalServer(zmqpp::context &ctx, const std::string &server_name);

    FExternalServer(const FExternalServer &) = delete;

    FExternalServer &operator=(const FExternalServer &) = delete;

    ~FExternalServer() = default;

    /**
    * Connect to the server.
    */
    bool connect();

    /**
    * Disconnect from the server.
    */
    bool disconnect();

    /**
    * Check if is connected to the server.
    */
    bool isConnected() const;

    /**
    * Returns the device's name.
    */
    const std::string &name() const;

  private:

    /**
    * Send a message to the `backend_` server and wait for a response.
    * The response shall be either "OK" or "KO"
    */
    bool send_to_backend(zmqpp::message &m);

    /**
    * A socket to talk to the backend server.
    */
    zmqpp::socket backend_;

    std::string name_;
};
}
}
