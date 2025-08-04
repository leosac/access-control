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

#include "AuthDBInstance.hpp"
#include "core/CoreUtils.hpp"
#include "tools/db/DBService.hpp"
#include "tools/log.hpp"
#include "tools/Colorize.hpp"
#include "core/auth/Auth.hpp"
#include <boost/algorithm/string/join.hpp>
#include <zmqpp/zmqpp.hpp>

using namespace Leosac::Module::Auth;
using namespace Leosac::Auth;

AuthDBInstance::AuthDBInstance(zmqpp::context &ctx,
                               const std::string &auth_ctx_name,
                               const std::list<std::string> &auth_sources_names,
                               const std::string &auth_target_name,
                               CoreUtilsPtr core_utils)
    : db_service_(std::make_shared<DBService>(core_utils->database()))
    , bus_push_(ctx, zmqpp::socket_type::push)
    , bus_sub_(ctx, zmqpp::socket_type::sub)
    , name_(auth_ctx_name)
    , target_name_(auth_target_name)
    , core_utils_(core_utils)
{
    bus_push_.connect("inproc://zmq-bus-pull");
    bus_sub_.connect("inproc://zmq-bus-pub");
    bus_sub_.subscribe("KERNEL");
    
    for (const auto &auth_source : auth_sources_names) {
        bus_sub_.subscribe("S_" + auth_source);
    }

    INFO("AuthDB Instance (" << auth_ctx_name << ") created and is subscribed to "
                             << boost::algorithm::join(auth_sources_names, ", "));
}

AuthDBInstance::~AuthDBInstance() {
    INFO("AuthDBInstance (" << name_ << ") destroyed");
}

void AuthDBInstance::handle_bus_msg() {
    zmqpp::message msg;
    zmqpp::message auth_result_msg;

    bus_sub_.receive(msg);
    if(handle_kernel_msg(msg)) {
        return;
    }

    format_auth_result_msg(auth_result_msg);
    auto auth_result = handle_auth_msg(msg);

    update_and_log_auth_result_msg(auth_result, auth_result_msg);
    bus_push_.send(auth_result_msg);
}

bool AuthDBInstance::handle_kernel_msg(zmqpp::message &msg) {
    auto msg_copy = msg.copy();
    std::string tmp;
    msg_copy >> tmp;

    if (tmp == "KERNEL") {
        msg_copy >> tmp;
        if (tmp == "SIGHUP") {
            INFO("AuthDBInstance received SIGHUP");
            // TODO: Implement reload authdb config
        }
        return true;
    }
    return false;
}

AuthResult AuthDBInstance::handle_auth_msg(zmqpp::message &msg) {
    // TODO: Implement this
    return AuthResult(false, nullptr, nullptr);
}

void AuthDBInstance::format_auth_result_msg(zmqpp::message &msg) {
    msg << ("S_" + name_);
}

std::string AuthDBInstance::format_user_name(const AuthResult &auth_result) {
    using namespace Colorize;
    if (auth_result.user) {
        return Colorize::green(auth_result.user->username());
    } else {
        return Colorize::red("UNKNOWN_USER");
    }
}

void AuthDBInstance::update_and_log_auth_result_msg(const AuthResult &auth_result, zmqpp::message &auth_result_msg) {
    using namespace Colorize;
    std::string user = format_user_name(auth_result);

    if (auth_result.success) {
        auth_result_msg << Leosac::Auth::AccessStatus::GRANTED;
        INFO(Colorize::bold(name_)
            << " " << Colorize::green("GRANTED") << " access to target "
            << Colorize::underline(target_name_) << " for user " << user);
    } else {
        auth_result_msg << Leosac::Auth::AccessStatus::DENIED;
        INFO(Colorize::bold(name_)
            << " " << Colorize::red("DENIED") << " access to target "
            << Colorize::underline(target_name_) << " for user " << user);
    }
}