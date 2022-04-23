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

#include "AuthFileInstance.hpp"
#include "FileAuthSourceMapper.hpp"
#include "core/CoreUtils.hpp"
#include "core/Scheduler.hpp"
#include "core/SecurityContext.hpp"
#include "core/auth/Auth.hpp"
#include "core/auth/AuthSourceBuilder.hpp"
#include "core/auth/User.hpp"
#include "core/credentials/serializers/PolymorphicCredentialSerializer.hpp"
#include "exception/ExceptionsTools.hpp"
#include "core/audit/AuditFactory.hpp"
#include "core/audit/IAuthEvent.hpp"
#include "tools/Colorize.hpp"
#include "tools/log.hpp"
#include <boost/algorithm/string/join.hpp>

using namespace Leosac::Module::Auth;
using namespace Leosac::Auth;

AuthFileInstance::AuthFileInstance(zmqpp::context &ctx,
                                   std::string const &auth_ctx_name,
                                   const std::list<std::string> &auth_sources_names,
                                   std::string const &auth_target_name,
                                   std::string const &input_file,
                                   CoreUtilsPtr core_utils)
    : mapper_(std::make_shared<FileAuthSourceMapper>(input_file))
    , bus_push_(ctx, zmqpp::socket_type::push)
    , bus_sub_(ctx, zmqpp::socket_type::sub)
    , name_(auth_ctx_name)
    , target_name_(auth_target_name)
    , file_path_(input_file)
    , core_utils_(core_utils)
{
    bus_push_.connect("inproc://zmq-bus-pull");
    bus_sub_.connect("inproc://zmq-bus-pub");

    bus_sub_.subscribe("KERNEL");

    INFO("Auth instance (" << auth_ctx_name << ") subscribe to "
                           << boost::algorithm::join(auth_sources_names, ", "));
    for (const auto &auth_source : auth_sources_names)
        bus_sub_.subscribe("S_" + auth_source);
}

AuthFileInstance::~AuthFileInstance()
{
    INFO("AuthFileInstance down");
}

void AuthFileInstance::handle_bus_msg()
{
    using namespace Colorize;
    zmqpp::message msg;
    zmqpp::message auth_result_msg;

    bus_sub_.receive(msg);
    if (handle_kernel_message(msg))
        return;

    auth_result_msg << ("S_" + name_);
    auto auth_result = handle_auth(&msg);

    std::string log_user;
    // output user id if available.
    if (auth_result.user)
        log_user = Colorize::green(auth_result.user->username());
    else
        log_user = Colorize::red("UNKNOWN_USER");

    if (auth_result.success)
    {
        auth_result_msg << Leosac::Auth::AccessStatus::GRANTED;
        INFO(Colorize::bold(name_)
             << " " << Colorize::green("GRANTED") << " access to target "
             << Colorize::underline(target_name_) << " for " << log_user);
    }
    else
    {
        auth_result_msg << Leosac::Auth::AccessStatus::DENIED;
        INFO(Colorize::bold(name_)
             << " " << Colorize::red("DENIED") << " access to target "
             << Colorize::underline(target_name_) << " for " << log_user);
    }
    bus_push_.send(auth_result_msg);
}

zmqpp::socket &AuthFileInstance::bus_sub()
{
    return bus_sub_;
}

AuthResult AuthFileInstance::handle_auth(zmqpp::message *msg) noexcept
{
  AuthResult authres(false, nullptr, nullptr);
  try
  {
    std::lock_guard<std::mutex> guard(mutex_);

    AuthSourceBuilder build;
    Cred::ICredentialPtr auth_source = build.create(msg);
    DEBUG("Auth source OK... will map");
    mapper_->mapToUser(auth_source);
    DEBUG("Mapping done");
    assert(auth_source);

    auto cred_serialized = PolymorphicCredentialJSONStringSerializer::serialize(
        *auth_source, SystemSecurityContext::instance());
    INFO("Using Credential: " << cred_serialized);
    auto profile = mapper_->buildProfile(auth_source);

    if (!profile)
    {
      INFO("No profile was created from this auth source message.");
    }
    else if (target_name_.empty())
    {
      // check against default target
      authres = AuthResult(profile->isAccessGranted(std::chrono::system_clock::now(), nullptr),
          profile, auth_source->owner().get_eager());
    }
    else
    {
      AuthTargetPtr t(new AuthTarget(target_name_));
      authres = AuthResult(profile->isAccessGranted(std::chrono::system_clock::now(), t),
          profile, auth_source->owner().get_eager());
    }

    auto db = core_utils_->database();
    if (db)
    {
      auto audit = Audit::Factory::AuthEvent(db, auth_source, target_name_);
      audit->event_mask(authres.success ? Audit::EventType::AUTH_GRANTED : Audit::EventType::AUTH_DENIED);
      audit->finalize();
    }
  }
  catch (std::exception &e)
  {
    WARN("Exception when handling authentication request.");
    log_exception(e);
  }

  return authres;
}

std::string AuthFileInstance::auth_file_content() const
{
    std::ifstream t(file_path_);
    std::stringstream buffer;
    buffer << t.rdbuf();

    return buffer.str();
}

const std::string &AuthFileInstance::auth_file_name() const
{
    return file_path_;
}

void AuthFileInstance::reload_auth_config()
{
    // The idea is to build a new mapper in an other thread
    // and swap it with the current mapper once it is built.
    // This is because building a new mapper can take a while.

    // We keep a shared_ptr to "this" in order to avoid dangling pointer to
    // a non-existent instance (for example if the module was shutdown between
    // the scheduling of the task and its execution).
    auto self      = shared_from_this();
    auto file_path = file_path_;
    auto task      = Tasks::GenericTask::build([self, file_path]() {
        try
        {
            auto mapper = std::make_shared<FileAuthSourceMapper>(file_path);
            {
                std::lock_guard<std::mutex> guard(self->mutex_);
                self->mapper_ = mapper;
                INFO("AuthFileInstance config reloaded.");
            }
            return true;
        }
        catch (const std::exception &e)
        {
            WARN("Problem when reloading AuthFileInstance configuration: "
                 << e.what());
            return false;
        }
    });
    core_utils_->scheduler().enqueue(task, TargetThread::POOL);
}

bool AuthFileInstance::handle_kernel_message(const zmqpp::message &msg)
{
    auto cp = msg.copy();
    std::string tmp;
    cp >> tmp;

    if (tmp == "KERNEL")
    {
        cp >> tmp;
        if (tmp == "SIGHUP")
        {
            reload_auth_config();
        }
        return true;
    }
    return false;
}
