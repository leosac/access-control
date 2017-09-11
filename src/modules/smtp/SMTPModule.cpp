/*
    Copyright (C) 2014-2016 Leosac

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
#include "SMTPAuditSerializer.hpp"
#include "SMTPServerInfoSerializer.hpp"
#include "SMTPServiceImpl.hpp"
#include "core/CoreUtils.hpp"
#include "core/GetServiceRegistry.hpp"
#include "core/UserSecurityContext.hpp"
#include "core/audit/IWSAPICall.hpp"
#include "core/audit/serializers/JSONService.hpp"
#include "core/auth/Auth.hpp"
#include "modules/smtp/SMTPAudit_odb.h"
#include "modules/smtp/SMTPConfig_odb.h"
#include "modules/websock-api/ExceptionConverter.hpp"
#include "modules/websock-api/Exceptions.hpp"
#include "modules/websock-api/Service.hpp"
#include "tools/Conversion.hpp"
#include "tools/MyTime.hpp"
#include "tools/registry/GlobalRegistry.hpp"
#include <boost/asio.hpp>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::SMTP;

SMTPModule::SMTPModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                       const boost::property_tree::ptree &cfg, CoreUtilsPtr utils)
    : AsioModule(ctx, pipe, cfg, utils)
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
    process_config();

    auto audit_serializer_service =
        utils_->service_registry().get_service<Audit::Serializer::JSONService>();
    ASSERT_LOG(audit_serializer_service,
               "Cannot retrieve Audit::Serializer::JSONService.");

    audit_serializer_service->register_serializer<SMTPAudit>(
        [](const SMTPAudit &audit, const SecurityContext &sc) -> json {
            return SMTPAuditSerializer::serialize(audit, sc);
        });

    // We're now mostly constructed, time to advertise our services.
    get_service_registry().register_service<SMTPService>(
        std::make_unique<SMTPServiceImpl>(*this));
}

SMTPModule::~SMTPModule()
{
    curl_global_cleanup();
    auto audit_serializer_service =
        utils_->service_registry().get_service<Audit::Serializer::JSONService>();
    ASSERT_LOG(audit_serializer_service,
               "Cannot retrieve Audit::Serializer::JSONService.");
    audit_serializer_service->unregister_serializer<SMTPAudit>();

    // Make sure we properly unregister the service.
    while (!get_service_registry().unregister_service<SMTPService>())
        ;
}

void SMTPModule::process_config()
{
    if ((use_database_ = config_.get<bool>("module_config.use_database", false)))
    {
        try
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
        }
        catch (const odb::exception &e)
        {
            WARN("SMTP module failed to load database configure. Using default "
                 "config.");
        }
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
            server.from = itr.second.get<std::string>("from", "leosac@leosac.com");
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
        register_ws_handlers();
}

bool SMTPModule::prepare_curl(const MailInfo &mail)
{
    if (smtp_config_->servers().size() == 0)
        WARN("Cannot send mail titled " << Colorize::cyan(mail.title)
                                        << ". No SMTP server configured.");
    if (mail.to.size() == 0)
    {
        WARN("No recipient for mail titled " << Colorize::cyan(mail.title) << '.');
        return false;
    }

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
    schema_version cv(schema_catalog::current_version(*db, "module_smtp"));
    if (v == 0)
    {
        transaction t(db->begin());
        schema_catalog::create_schema(*db, "module_smtp");

        SMTPConfig cfg;
        SMTPServerInfo srv;
        srv.url      = "smtp://mail.leosac.com";
        srv.username = "leosac-mail";
        srv.from     = "leosac@leosac.com";
        cfg.server_add(srv);
        db->persist(cfg);

        t.commit();
    }
    else if (v < cv)
    {
        INFO("SMTP Module performing database migration. Going from version "
             << v << " to version " << cv);
        transaction t(db->begin());
        schema_catalog::migrate(*db, cv, "module_smtp");
        t.commit();
    }
}

json SMTPModule::handle_ws_smtp_getconfig(const WebSockAPI::RequestContext &req_ctx,
                                          const json &)
{
    json ret = json::array();
    for (const auto &server_cfg : smtp_config_->servers())
    {
        ret.push_back(SMTPServerInfoJSONSerializer::serialize(server_cfg,
                                                              req_ctx.security_ctx));
    }
    return ret;
}

json SMTPModule::handle_ws_smtp_setconfig(const WebSockAPI::RequestContext &req_ctx,
                                          const json &req)
{
    SMTPConfigUPtr cfg = std::make_unique<SMTPConfig>();
    for (const auto &server : req.at("servers"))
    {
        SMTPServerInfo srv_info;
        SMTPServerInfoJSONSerializer::unserialize(srv_info, server,
                                                  req_ctx.security_ctx);
        cfg->server_add(srv_info);
    }

    {
        auto db = utils_->database();
        odb::transaction t(db->begin());
        auto smtp_audit = SMTPAudit::create(db, req_ctx.audit);

        if (smtp_config_->id()) // Maybe we don't have a persisted configuration yet
            db->erase<SMTPConfig>(smtp_config_->id());

        db->persist(*cfg);
        smtp_audit->finalize();
        t.commit();
        smtp_config_ = std::move(cfg);
    }

    return {};
}

json SMTPModule::handle_ws_smtp_sendmail(const WebSockAPI::RequestContext &,
                                         const json &req)
{
    MailInfo mail;

    mail.body  = req.at("body");
    mail.title = req.at("subject");
    for (const auto &recipient : req.at("to"))
        mail.to.push_back(recipient);

    return {{"sent", (prepare_curl(mail))}};
}

void SMTPModule::on_service_event(const service_event::Event &e)
{
    // We are interested in WebSockAPI::Service registration so we can
    // register our websocket handler against it.
    if (e.type() == service_event::REGISTERED)
    {
        auto &service_registered_event =
            assert_cast<const service_event::ServiceRegistered &>(e);
        if (service_registered_event.interface_type() ==
            boost::typeindex::type_id<WebSockAPI::Service>())
        {
            // The service-event handler is invoked in the Websocket thread.
            // We want the registration to happen from our own thread, otherwise
            // it will deadlock (because the ws thread event loop is blocked
            // invoking this handler, that in turn would rely on a callable being
            // invoked from the ws thread event loop.
            io_service_.post([this]() {
                // We don't care about registering WS handler if we don't use the
                // database.
                if (!use_database_)
                    return;
                register_ws_handlers();
            });
        }
    }
}

void SMTPModule::register_ws_handlers()
{
    auto ws_srv = get_service_registry().get_service<Module::WebSockAPI::Service>();
    if (ws_srv)
    {
        // GetConfig

        bool ret = ws_srv->register_asio_handler_permission(
            [this](const WebSockAPI::RequestContext &req_ctx) {
                return handle_ws_smtp_getconfig(req_ctx,
                                                req_ctx.original_msg.content);
            },
            wshandler_getconfig, SecurityContext::Action::SMTP_GETCONFIG,
            io_service_);
        if (!ret)
            WARN("Cannot register SMTP WS handler: " << wshandler_getconfig);

        // SetConfig
        ret = ws_srv->register_asio_handler_permission(
            [this](const WebSockAPI::RequestContext &req_ctx) {
                return handle_ws_smtp_setconfig(req_ctx,
                                                req_ctx.original_msg.content);
            },
            wshandler_setconfig, SecurityContext::Action::SMTP_SETCONFIG,
            io_service_);
        if (!ret)
            WARN("Cannot register SMTP WS handler: " << wshandler_setconfig);

        // SendMail
        ret = ws_srv->register_asio_handler_permission(
            [this](const WebSockAPI::RequestContext &req_ctx) {
                return handle_ws_smtp_sendmail(req_ctx,
                                               req_ctx.original_msg.content);
            },
            wshandler_sendmail, SecurityContext::Action::SMTP_SENDMAIL, io_service_);
        if (!ret)
            WARN("Cannot register SMTP WS handler: " << wshandler_sendmail);
    }
    else
    {
        INFO("Cannot register Websocket handlers. Service is not (yet) available.");
    }
}

void SMTPModule::async_send_mail(const MailInfo &mail)
{
    io_service_.post([this, mail]() { prepare_curl(mail); });
}
