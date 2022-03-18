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

#include "FExternalServer.hpp"

using namespace Leosac::Hardware;

FExternalServer::FExternalServer(zmqpp::context &ctx, std::string const &server_name)
    : backend_(ctx, zmqpp::socket_type::req)
    , name_(server_name)
{
    backend_.connect("inproc://" + server_name);
}

bool FExternalServer::connect()
{
    zmqpp::message msg;
    msg << "CONNECT";

    return send_to_backend(msg);
}

bool FExternalServer::disconnect()
{
    zmqpp::message msg;
    msg << "DISCONNECT";

    return send_to_backend(msg);
}

bool FExternalServer::send_to_backend(zmqpp::message &msg)
{
    std::string rep;
    backend_.send(msg);
    backend_.receive(rep);

    assert(rep == "OK" || rep == "KO");
    if (rep == "OK")
        return true;
    return false;
}

const std::string &FExternalServer::name() const
{
    return name_;
}
