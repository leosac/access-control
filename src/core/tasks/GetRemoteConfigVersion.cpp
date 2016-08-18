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

#include <tools/log.hpp>
#include "GetRemoteConfigVersion.hpp"
#include "core/kernel.hpp"
#include <zmqpp/curve.hpp>

using namespace Leosac;

Tasks::GetRemoteConfigVersion::GetRemoteConfigVersion(const std::string &endpoint,
                                                      const std::string &pubkey)
    : config_version_(0)
    , endpoint_(endpoint)
    , pubkey_(pubkey)
{
    INFO("Creating GetRemoteConfigVersion task. Guid = " << get_guid());
}

bool Tasks::GetRemoteConfigVersion::do_run()
{
    zmqpp::context ctx;
    zmqpp::socket sock(ctx, zmqpp::socket_type::dealer);
    auto kp = zmqpp::curve::generate_keypair();

    sock.set(zmqpp::socket_option::curve_secret_key, kp.secret_key);
    sock.set(zmqpp::socket_option::curve_public_key, kp.public_key);
    sock.set(zmqpp::socket_option::curve_server_key, pubkey_);
    sock.set(zmqpp::socket_option::linger, 0);
    sock.connect(endpoint_);

    sock.send("CONFIG_VERSION");

    zmqpp::message response;
    zmqpp::poller p;
    p.add(sock);

    p.poll(timeout);
    if (p.has_input(sock))
    {
        sock.receive(response);
        response >> config_version_;
        INFO("Remote configuration version = " << config_version_);
        return true;
    }
    ERROR("Failed to receive response from remote server in due time");
    return false;
}
