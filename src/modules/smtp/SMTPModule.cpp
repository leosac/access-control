/*
    Copyright (C) 2014-2016 Islog

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

#include "SMTPModule.hpp"
#include "SMTPConfig.hpp"
#include "SMTPConfig_odb.h"
#include "SMTPServerInfoSerializer.hpp"
#include "core/CoreUtils.hpp"
#include "core/UserSecurityContext.hpp"
#include "core/audit/IWSAPICall.hpp"
#include "core/auth/Auth.hpp"
#include "modules/websock-api/Exceptions.hpp"
#include "modules/websock-api/Facade.hpp"
#include "modules/websock-api/Messages.hpp"
#include "tools/AssertCast.hpp"
#include "tools/Conversion.hpp"
#include "tools/Mail.hpp"
#include "tools/MyTime.hpp"
#include "tools/db/database.hpp"
#include "tools/registry/GlobalRegistry.hpp"
#include <curl/curl.h>
#include <json.hpp>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::SMTP;

SMTPModule::SMTPModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                       const boost::property_tree::ptree &cfg, CoreUtilsPtr utils)
    : BaseModule(ctx, pipe, cfg, utils)
    , bus_sub_(ctx, zmqpp::socket_type::sub)
{
    int ret;
    long flags = 0;

    if (config_.get<bool>("module_config.want_ssl", true))
        flags |= CURL_GLOBAL_SSL;
    if ((ret = curl_global_init(flags)) != 0)
    {
        throw std::runtime_error("Failed to initialize curl: return code: " +
                                 std::to_string(ret));
    }
    bus_sub_.connect("inproc://zmq-bus-pub");
    bus_sub_.subscribe("SERVICE.MAILER");
    process_config();
    reactor_.add(bus_sub_, std::bind(&SMTPModule::handle_msg_bus, this));
}

SMTPModule::~SMTPModule()
{
    curl_global_cleanup();
}

void SMTPModule::handle_msg_bus()
{
    zmqpp::message msg;
    std::string key;

    bus_sub_.receive(msg);
    if (msg.parts() != 2)
    {
        WARN("Unexpected message content.");
        return;
    }
    msg.pop_front();
    msg >> key;
    try
    {
        MailInfo mail = GlobalRegistry::get<MailInfo>(key);
        prepare_curl(mail);
    }
    catch (const RegistryKeyNotFoundException &e)
    {
        WARN("SMTP: cannot retrieve MailInfo from GlobalRegistry. Key was: " << key);
    }
}

void SMTPModule::process_config()
{
    if ((use_database_ = config_.get<bool>("module_config.use_database", false)))
    {
        setup_database();
        // Load config from database.
        int count = 0;
        odb::transaction t(utils_->database()->begin());
        odb::result<SMTPConfig> result(utils_->database()->query<SMTPConfig>());
        for (const auto &cfg : result)
        {
            smtp_config_ = std::make_unique<SMTPConfig>(cfg);
            count++;
        }
        t.commit();
        ASSERT_LOG(count == 0 || count == 1,
                   "We have more than one SMTPConfig entry in the database.");
        INFO("SMTP module using SQL database for configuration.");
        if (!smtp_config_)
            smtp_config_ = std::make_unique<SMTPConfig>();
    }
    else
    {
        smtp_config_ = std::make_unique<SMTPConfig>();
        for (auto &&itr : config_.get_child("module_config.servers"))
        {
            SMTPServerInfo server;
            server.url  = itr.second.get<std::string>("url");
            server.from = itr.second.get<std::string>("from", "leosac@islog.com");
            server.verify_host   = itr.second.get<bool>("verify_host", true);
            server.verify_peer   = itr.second.get<bool>("verify_peer", true);
            server.CA_info_file_ = itr.second.get<std::string>("ca_file", "");

            INFO("SMTP module server: "
                 << Colorize::green(server.url)
                 << ", verify_host: " << Colorize::green(server.verify_host)
                 << ", verify_peer: " << Colorize::green(server.verify_peer)
                 << ", ca_info: " << Colorize::green(server.CA_info_file_) << ")");
            smtp_config_->server_add(server);
        }
    }

    if (use_database_)
    {
        websocket_api = std::make_unique<WebSockAPI::Facade>(reactor_, utils_);
        auto handler  = std::bind(&SMTPModule::handle_websocket_message, this,
                                 std::placeholders::_1, std::placeholders::_2);
        websocket_api->register_handler(wshandler_getconfig, handler);
        websocket_api->register_handler(wshandler_setconfig, handler);
        websocket_api->register_handler(wshandler_sendmail, handler);
    }
}

bool SMTPModule::prepare_curl(const MailInfo &mail)
{
    if (smtp_config_->servers().size() == 0)
        WARN("Cannot send mail titled " << Colorize::cyan(mail.title)
                                        << ". No SMTP server configured.");
    for (const auto &target : smtp_config_->servers())
    {
        if (!target.enabled)
            continue;

        auto curl = curl_easy_init();
        if (curl)
        {
            if (!target.CA_info_file_.empty())
                curl_easy_setopt(curl, CURLOPT_CAINFO, target.CA_info_file_.c_str());
            if (!target.verify_host)
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            if (!target.verify_peer)
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            if (target.username.size())
                curl_easy_setopt(curl, CURLOPT_USERNAME, target.username.c_str());
            if (target.password.size())
                curl_easy_setopt(curl, CURLOPT_PASSWORD, target.password.c_str());
            if (target.from.size())
                curl_easy_setopt(curl, CURLOPT_MAIL_FROM, target.from.c_str());

            ASSERT_LOG(target.url.size(), "No mail server url.");
            curl_easy_setopt(curl, CURLOPT_URL, target.url.c_str());

            curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, target.ms_timeout);

            bool sent = send_mail(curl, mail);
            curl_easy_cleanup(curl);
            if (sent)
                return true;
        }
        else
        {
            ERROR("Cannot initialize curl_easy.");
        }
    }
    return false;
}

struct UploadStatus
{
    const MailInfo &mail;
    int counter;
};

/**
 * Return a string representation of what the whole mail would look like.
 */
static std::string build_mail_str(const MailInfo &mail)
{
    std::stringstream ss;

    ASSERT_LOG(mail.to.size(), "No recipients for mail.");
    ss << "Date: " << to_local_rfc2822(std::chrono::system_clock::now()) << "\r\n";
    ss << "To: " << mail.to.at(0) << "\r\n";

    ss << "Subject: " << mail.title << "\r\n";
    ss << "\r\n"; // empty line to divide headers from body, see RFC5322
    ss << mail.body << "\r\n\r\n";

    return ss.str();
}

/**
 * Callback for libcurl.
 *
 * CURL invokes this to get the data it should send to the server.
 */
static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
    UploadStatus *st = static_cast<UploadStatus *>(userp);
    ASSERT_LOG(st, "UploadStatus is null.");

    std::string content = build_mail_str(st->mail);
    auto wanted         = size * nmemb;
    auto available      = content.size() - st->counter;
    auto to_transfer    = std::min(available, wanted);

    std::memset(ptr, 0, wanted);
    std::memcpy(ptr, &content[0] + st->counter, to_transfer);
    st->counter += to_transfer;

    return to_transfer;
}

bool SMTPModule::send_mail(CURL *curl, const MailInfo &mail)
{
    ASSERT_LOG(curl, "CURL pointer is null.");

    struct curl_slist *recipients = NULL;
    for (const auto &recipient : mail.to)
        recipients = curl_slist_append(recipients, recipient.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    UploadStatus status{.mail = mail, .counter = 0};

    curl_easy_setopt(curl, CURLOPT_READFUNCTION, &read_callback);
    curl_easy_setopt(curl, CURLOPT_READDATA, &status);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    auto res = curl_easy_perform(curl);
    curl_slist_free_all(recipients);
    if (res != CURLE_OK)
    {
        WARN("curl_easy_perform() failed: " << curl_easy_strerror(res));
        return false;
    }
    else
    {
        INFO("Mail titled " << Colorize::cyan(mail.title) << " has been sent.");
        return true;
    }
}

void SMTPModule::setup_database()
{
    using namespace odb;
    using namespace odb::core;
    auto db          = utils_->database();
    schema_version v = db->schema_version("module_smtp");
    if (v == 0)
    {
        transaction t(db->begin());
        schema_catalog::create_schema(*db, "module_smtp");


        // Create a dummy SMTP server config
        // todo remove this

        SMTPConfig cfg;
        SMTPServerInfo srv;
        srv.url      = "smtp://mail.leosac.com";
        srv.username = "leosac-mail";
        srv.from     = "leosac@leosac.com";
        cfg.server_add(srv);
        db->persist(cfg);

        t.commit();
    }
}

WebSockAPI::ServerMessage SMTPModule::handle_websocket_message(
    const WebSockAPI::ModuleRequestContext &request_ctx,
    const WebSockAPI::ClientMessage &msg)
{
    WebSockAPI::ServerMessage response;
    response.status_code = APIStatusCode::UNKNOWN;
    response.content     = {};
    try
    {
        response.uuid = msg.uuid;
        response.type = msg.type;

        if (msg.type == wshandler_getconfig)
        {
            if (request_ctx.security_ctx->check_permission(
                    SecurityContext::Action::SMTP_GETCONFIG, {}))
            {
                response.content =
                    handle_ws_smtp_getconfig(request_ctx, msg.content);
                response.status_code = APIStatusCode::SUCCESS;
            }
            else
                throw WebSockAPI::PermissionDenied();
        }
        else if (msg.type == wshandler_setconfig)
        {
            if (request_ctx.security_ctx->check_permission(
                    SecurityContext::Action::SMTP_SETCONFIG, {}))
            {
                response.content =
                    handle_ws_smtp_setconfig(request_ctx, msg.content);
                response.status_code = APIStatusCode::SUCCESS;
            }
            else
                throw WebSockAPI::PermissionDenied();
        }
        else if (msg.type == wshandler_sendmail)
        {
            if (request_ctx.security_ctx->check_permission(
                    SecurityContext::Action::SMTP_SENDMAIL, {}))
            {
                response.content = handle_ws_smtp_sendmail(request_ctx, msg.content);
                response.status_code = APIStatusCode::SUCCESS;
            }
            else
                throw WebSockAPI::PermissionDenied();
        }
    }
    catch (const WebSockAPI::PermissionDenied &e)
    {
        response.status_code   = APIStatusCode::PERMISSION_DENIED;
        response.status_string = e.what();
    }
    catch (const LEOSACException &e)
    {
        WARN("Leosac specific exception has been caught: " << e.what() << std::endl
                                                           << e.trace().str());
        response.status_code   = APIStatusCode::GENERAL_FAILURE;
        response.status_string = e.what(); // todo Maybe remove in production.
    }
    catch (const odb::exception &e)
    {
        ERROR("Database Error: " << e.what());
        response.status_code   = APIStatusCode::DATABASE_ERROR;
        response.status_string = "Database Error: " + std::string(e.what());
    }
    catch (const std::exception &e)
    {
        WARN("Exception when processing request: " << e.what());
        response.status_code   = APIStatusCode::GENERAL_FAILURE;
        response.status_string = e.what();
    }
    return response;
}

json SMTPModule::handle_ws_smtp_getconfig(
    const WebSockAPI::ModuleRequestContext &req_ctx, const json &)
{
    json ret = json::array();
    for (const auto &server_cfg : smtp_config_->servers())
    {
        ret.push_back(SMTPServerInfoJSONSerializer::serialize(
            server_cfg, *req_ctx.security_ctx));
    }
    return ret;
}

json SMTPModule::handle_ws_smtp_setconfig(
    const WebSockAPI::ModuleRequestContext &req_ctx, const json &req)
{
    SMTPConfigUPtr cfg = std::make_unique<SMTPConfig>();
    for (const auto &server : req.at("servers"))
    {
        SMTPServerInfo srv_info;
        SMTPServerInfoJSONSerializer::unserialize(srv_info, server,
                                                  *req_ctx.security_ctx);
        cfg->server_add(srv_info);
    }

    {
        auto db = utils_->database();
        odb::transaction t(db->begin());
        if (smtp_config_->id()) // May we don't have a persisted configuration yet
            db->erase<SMTPConfig>(smtp_config_->id());

        db->persist(*cfg);
        t.commit();
        smtp_config_ = std::move(cfg);
    }

    return {};
}

json SMTPModule::handle_ws_smtp_sendmail(
    const WebSockAPI::ModuleRequestContext &req_ctx, const json &req)
{
    MailInfo mail;

    mail.body  = req.at("body");
    mail.title = req.at("subject");
    for (const auto &recipient : req.at("to"))
        mail.to.push_back(recipient);

    if (prepare_curl(mail))
    {
        return {{"sent", true}};
    }
    return {{"sent", false}};
}
