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
#include "core/auth/User.hpp"
#include "core/auth/User_odb.h"
#include "core/auth/AuthSourceBuilder.hpp"
#include "core/credentials/ICredential.hpp"
#include "core/credentials/RFIDCard.hpp"
#include "core/credentials/PinCode.hpp"
#include "core/credentials/RFIDCardPin.hpp"
#include "core/credentials/serializers/PolymorphicCredentialSerializer.hpp"
#include "core/credentials/RFIDCard_odb.h"
#include "core/SecurityContext.hpp"
#include "exception/ExceptionsTools.hpp"
#include <boost/algorithm/string/join.hpp>
#include <zmqpp/zmqpp.hpp>
#include <odb/transaction.hxx>
#include <odb/query.hxx>

using namespace Leosac::Module::Auth;
using namespace Leosac::Auth;
using namespace Leosac::Cred;

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
    auto auth_result = handle_auth(&msg);

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

AuthResult AuthDBInstance::handle_auth(zmqpp::message *msg) noexcept {
    AuthResult auth_result(false, nullptr, nullptr);

    try {
        std::lock_guard<std::mutex> guard(mutex_);
        
        Cred::ICredentialPtr credentials = get_db_credentials(msg);
        if (!credentials) {
            return auth_result;
        }

        log_credentials(credentials); // Temporary

        //TODO: Finish this

    } catch (std::exception &e) {
        WARN("Error while handling auth request: " << e.what());
        log_exception(e);
    }

    return auth_result;
}

ICredentialPtr AuthDBInstance::get_db_credentials(zmqpp::message *msg) {
    AuthSourceBuilder builder;
    Cred::ICredentialPtr auth_source = builder.create(msg);
    Cred::ICredentialPtr db_credentials = nullptr;

    if (auto rfid_card = std::dynamic_pointer_cast<Cred::RFIDCard>(auth_source)) {
        db_credentials = find_credentials_by_card_id(rfid_card->card_id(), rfid_card->nb_bits());
    } else if (auto pin_code = std::dynamic_pointer_cast<Cred::PinCode>(auth_source)) {
        INFO("Pin code auth source not supported yet");
    } else if (auto card_pin = std::dynamic_pointer_cast<Cred::RFIDCardPin>(auth_source)) {
        INFO("Card pin auth source not supported yet");
    } else {
        WARN("Unknown credential type");
    }

    return db_credentials;
}

ICredentialPtr AuthDBInstance::find_credentials_by_card_id(const std::string &card_id, const int nb_bits) const {
    INFO("Searching for credentials by card id: " << card_id << " with " << nb_bits << " bits");

    try {
        using namespace odb;
        using namespace odb::core;
        using Query = odb::query<Cred::RFIDCard>;

        auto db = core_utils_->database();
        odb::transaction t(db->begin());

        Query q(Query::card_id == card_id && Query::nb_bits == nb_bits);
        auto result = db->query<Cred::RFIDCard>(q);

        for (const auto &card : result) {
            if (card.validity().is_valid()) {
                t.commit();
                return std::make_shared<Cred::RFIDCard>(card);
            } else {
                INFO("RFIDCard is not enabled (validity check failed).");
            }
        }

        t.commit();
        return nullptr;

    } catch (const std::exception &e) {
        WARN("Error finding credentials by card id: " << e.what());
        log_exception(e);
        return nullptr;
    }
}

::Leosac::Auth::UserPtr AuthDBInstance::get_user(Cred::ICredentialPtr &credentials) {
    using namespace odb;
    using namespace odb::core;
    ::Leosac::Auth::UserPtr user = nullptr;

    auto db = core_utils_->database();
    odb::transaction t(db->begin());

    if (auto owner_lazy = credentials->owner()) {
        if (auto owner = owner_lazy.load()) {
            user = owner;
        } else {
            INFO("User does not exist in database");
        }
    } else {
        INFO("These credentials do not have an owner");
    }

    t.commit();

    return user;
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

void AuthDBInstance::log_credentials(Cred::ICredentialPtr &credentials) {
    using namespace odb;
    using namespace odb::core;
    auto db = core_utils_->database();
    odb::transaction t(db->begin());
    
    std::string cred_serialized;
    cred_serialized = PolymorphicCredentialJSONStringSerializer::serialize(
        *credentials, SystemSecurityContext::instance());
    INFO("Using Credential: " << cred_serialized);
    
    t.commit();
}