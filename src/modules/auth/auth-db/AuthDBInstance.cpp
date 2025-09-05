/*
    Copyright (C) 2014-2025 Leosac

    This file is part of Leosac Access Control.

    Leosac Access Control is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leosac Access Control is distributed in the hope that it will be useful,
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
#include "core/auth/AuthFwd.hpp"
#include "core/auth/User.hpp"
#include "core/auth/User_odb.h"
#include "core/auth/AuthSourceBuilder.hpp"
#include "core/auth/SimpleAccessProfile.hpp"
#include "core/auth/ProfileMerger.hpp"
#include "core/credentials/ICredential.hpp"
#include "core/credentials/RFIDCard.hpp"
#include "core/credentials/PinCode.hpp"
#include "core/credentials/RFIDCardPin.hpp"
#include "core/credentials/serializers/PolymorphicCredentialSerializer.hpp"
#include "core/credentials/RFIDCard_odb.h"
#include "core/SecurityContext.hpp"
#include "core/audit/AuthEvent.hpp"
#include "core/audit/AuditFactory.hpp"
#include "exception/ExceptionsTools.hpp"
#include <boost/algorithm/string/join.hpp>
#include <zmqpp/zmqpp.hpp>
#include <odb/transaction.hxx>
#include <odb/query.hxx>

using namespace Leosac::Module::Auth;
using namespace Leosac::Auth;
using namespace Leosac::Cred;
using namespace ::Leosac::Auth;

AuthDBInstance::AuthDBInstance(zmqpp::context &ctx,
                               const std::string &auth_ctx_name,
                               const std::list<std::string> &auth_sources_names,
                               const std::string &auth_target_name,
                               CoreUtilsPtr core_utils,
                               const int bits_low_threshold,
                               const int bits_high_threshold)
    : db_service_(std::make_shared<DBService>(core_utils->database()))
    , bus_push_(ctx, zmqpp::socket_type::push)
    , bus_sub_(ctx, zmqpp::socket_type::sub)
    , name_(auth_ctx_name)
    , target_name_(auth_target_name)
    , core_utils_(core_utils)
    , bits_low_threshold_(bits_low_threshold)
    , bits_high_threshold_(bits_high_threshold)
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
 
    if (!auth_result.ignore) {
        update_and_log_auth_result_msg(auth_result, auth_result_msg);
        bus_push_.send(auth_result_msg);
    }
}

bool AuthDBInstance::handle_kernel_msg(zmqpp::message &msg) {
    auto msg_copy = msg.copy();
    std::string tmp;
    msg_copy >> tmp;

    if (tmp == "KERNEL") {
        msg_copy >> tmp;
        if (tmp == "SIGHUP") {
            INFO("AuthDBInstance received SIGHUP");
            // TODO: Implement reload authdb config?
        }
        return true;
    }
    return false;
}

AuthResult AuthDBInstance::handle_auth(zmqpp::message *msg) noexcept {
    AuthResult auth_result(false, false, nullptr, nullptr);

    try {
        std::lock_guard<std::mutex> guard(mutex_);
        
        CredResult cred_result = get_db_credentials(msg);
        if (cred_result.ignore) {
            auth_result.ignore = true;
            return auth_result;
        }

        Cred::ICredentialPtr credentials = cred_result.db_credentials;
        if (!credentials) {
            return auth_result;
        }

        log_credentials(credentials); // Temporary

        UserPtr user = get_user(credentials);
        IAccessProfilePtr profile = build_profile(user, credentials);

        if (profile) {
            bool access_granted = is_access_granted(profile);
            auth_result = AuthResult(access_granted, false, profile, user);
        }

        log_auth_event(auth_result, credentials);

    } catch (std::exception &e) {
        WARN("Error while handling auth request: " << e.what());
        log_exception(e);
    }

    return auth_result;
}

CredResult AuthDBInstance::get_db_credentials(zmqpp::message *msg) {
    AuthSourceBuilder builder;
    Cred::ICredentialPtr auth_source = builder.create(msg);
    CredResult cred_result(false, nullptr);

    if (auto rfid_card = std::dynamic_pointer_cast<Cred::RFIDCard>(auth_source)) {
        cred_result = find_credentials_by_card_id(rfid_card->card_id(), rfid_card->nb_bits());
    } else if (auto pin_code = std::dynamic_pointer_cast<Cred::PinCode>(auth_source)) {
        INFO("Pin code auth source not supported yet");
    } else if (auto card_pin = std::dynamic_pointer_cast<Cred::RFIDCardPin>(auth_source)) {
        INFO("Card pin auth source not supported yet");
    } else {
        WARN("Unknown credential type");
    }

    return cred_result;
}

CredResult AuthDBInstance::find_credentials_by_card_id(const std::string &card_id, const int nb_bits) const {
    INFO("Searching for credentials by card id: " << card_id << " with " << nb_bits << " bits");

    if (is_noise(nb_bits)) {
        INFO("Number of bits, " << nb_bits << ", is configured as noise, ignoring.");
        return CredResult(true, nullptr);
    }

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
                return CredResult(false, std::make_shared<Cred::RFIDCard>(card));
            } else {
                INFO("RFIDCard is not enabled (validity check failed).");
            }
        }

        t.commit();
        return CredResult(false, nullptr);

    } catch (const std::exception &e) {
        WARN("Error finding credentials by card id: " << e.what());
        log_exception(e);
        return CredResult(false, nullptr);
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

::Leosac::Auth::IAccessProfilePtr AuthDBInstance::build_profile(UserPtr &user, ICredentialPtr &credentials) {
    try {
        std::vector<IAccessProfilePtr> profiles;

        using namespace odb;
        using namespace odb::core;
        auto db = core_utils_->database();
        odb::transaction t(db->begin());

        auto mappings = db->query<Tools::ScheduleMapping>();

        for (const auto &schedule_map : mappings) {
            if ((user && schedule_map.has_user_indirect(user)) || 
                (credentials && schedule_map.has_cred(credentials->id()))) {
                create_profile_from_schedule_mapping(schedule_map, profiles);
            }
        }

        t.commit();
        
        if (profiles.empty()) {
            return nullptr;
        } else if (profiles.size() == 1) {
            return profiles.at(0);
        } else {
            ProfileMerger merger;
            auto merged_profile = profiles.at(0);
            for (size_t i = 1; i < profiles.size(); ++i) {
                merged_profile = merger.merge(merged_profile, profiles.at(i));
            }
            return merged_profile;
        }

    } catch (const std::exception &e) {
        WARN("AuthDBInstance::build_profile - Error building profile: " << e.what());
        return nullptr;
    }
}

void AuthDBInstance::create_profile_from_schedule_mapping(const Tools::ScheduleMapping &mapping, 
                                                         std::vector<IAccessProfilePtr> &profiles) {
    auto profile = std::make_shared<SimpleAccessProfile>();
    auto schedule = mapping.schedule().load();

    if (!schedule) {
        return;
    }

    add_doors_to_profile(mapping, profile, schedule);

    profiles.push_back(profile);
}

void AuthDBInstance::add_doors_to_profile(const Tools::ScheduleMapping &mapping,
                                          SimpleAccessProfilePtr &profile,
                                          const Leosac::Tools::IScheduleCPtr& schedule) {
    for (const auto &door : mapping.doors()) {
        if (auto lazy_door = door.load()) {
            std::string alias = lazy_door->alias();
            if (alias.empty()) {
                continue;
            }

            profile->addAccessSchedule(AuthTargetPtr(new AuthTarget(alias)), schedule);

            static const std::string ns_prefix = "my_leosac.";
            if (alias.rfind(ns_prefix, 0) != 0) {
                profile->addAccessSchedule(AuthTargetPtr(new AuthTarget(ns_prefix + alias)), schedule);
            }
        }
    }

    if (mapping.doors().empty()) {
        profile->addAccessSchedule(nullptr, schedule);
    }
}

bool AuthDBInstance::is_access_granted(IAccessProfilePtr &profile) {
    auto now = std::chrono::system_clock::now();

    if (target_name_.empty()) {
        return profile->isAccessGranted(now, nullptr);
    } else {
        AuthTargetPtr target(new AuthTarget(target_name_));
        return profile->isAccessGranted(now, target);
    }
}

void AuthDBInstance::log_auth_event(const AuthResult &auth_result, Cred::ICredentialPtr &credentials) {
    try {
        if (!target_name_.empty()) {
            using namespace odb;
            using namespace odb::core;
            auto db = core_utils_->database();
            odb::transaction t(db->begin());

            auto audit = Audit::Factory::AuthEvent(db, credentials, target_name_);
            audit->event_mask(auth_result.success ? Audit::EventType::AUTH_GRANTED : Audit::EventType::AUTH_DENIED);
            audit->finalize();

            t.commit();
        } else {
            INFO("No target name provided, skipping Audit AuthEvent creation");
        }
    } catch (const std::exception &e) {
        WARN("Failed to create AuthEvent: " << e.what());
    }
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
    INFO("Using Credentials: " << cred_serialized);
    
    t.commit();
}

bool AuthDBInstance::is_noise(const int nb_bits) const {
    if (bits_low_threshold_ != -1 && nb_bits <= bits_low_threshold_) {
        return true;
    }

    if (bits_high_threshold_ != -1 && nb_bits >= bits_high_threshold_) {
        return true;
    }

    return false;
}