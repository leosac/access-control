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

#include <core/auth/WiegandCard.hpp>
#include <core/auth/AuthSourceBuilder.hpp>
#include <core/auth/Auth.hpp>
#include <exception/ExceptionsTools.hpp>
#include "AuthFileInstance.hpp"
#include "tools/log.hpp"
#include "FileAuthSourceMapper.hpp"

using namespace Leosac::Module::Auth;
using namespace Leosac::Auth;

AuthFileInstance::AuthFileInstance(zmqpp::context &ctx,
        std::string const &auth_ctx_name,
        std::string const &auth_source_name,
        std::string const &auth_target_name,
        std::string const &input_file) :
        bus_push_(ctx, zmqpp::socket_type::push),
        bus_sub_(ctx, zmqpp::socket_type::sub),
        name_(auth_ctx_name),
        target_name_(auth_target_name),
        file_path_(input_file)
{
    bus_push_.connect("inproc://zmq-bus-pull");
    bus_sub_.connect("inproc://zmq-bus-pub");

    INFO("Auth instance (" << auth_ctx_name << ") subscribe to " << auth_source_name);
    bus_sub_.subscribe("S_" + auth_source_name);
}

AuthFileInstance::~AuthFileInstance()
{
    INFO("AuthFileInstance down");
}

void AuthFileInstance::handle_bus_msg()
{
    zmqpp::message auth_msg;
    zmqpp::message auth_result_msg;

    bus_sub_.receive(auth_msg);

    auth_result_msg << ("S_" + name_);
    if (handle_auth(&auth_msg))
    {
        auth_result_msg << Leosac::Auth::AccessStatus::GRANTED;
        INFO(name_ << " GRANTED access to target " << target_name_ << " for someone");
    }
    else
    {
        auth_result_msg << Leosac::Auth::AccessStatus::DENIED;
        INFO(name_ << " DENIED access to target " << target_name_ << " for someone");
    }
    bus_push_.send(auth_result_msg);
}

zmqpp::socket &AuthFileInstance::bus_sub()
{
    return bus_sub_;
}

bool AuthFileInstance::handle_auth(zmqpp::message *msg) noexcept
{
    try
    {
        AuthSourceBuilder build;
        IAuthenticationSourcePtr ptr = build.create(msg);
        FileAuthSourceMapper mapper(file_path_);

        mapper.mapToUser(ptr);
        if (!ptr->profile())
        {
            NOTICE("No profile were created from this auth source message.");
            return false;
        }
        if (target_name_.empty())
        {
            // check against default target
            return ptr->profile()->isAccessGranted(std::chrono::system_clock::now(), nullptr);
        }
        else
        {
            AuthTargetPtr t(new AuthTarget(target_name_));
            return ptr->profile()->isAccessGranted(std::chrono::system_clock::now(), t);
        }
    }
    catch (std::exception &e)
    {
        log_exception(e);
    }
    return false;
}
