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

#include "SMTP.hpp"
#include "SMTPConfig.hpp"
#include "SMTPConfig_odb.h"
#include "core/CoreUtils.hpp"
#include "core/UserSecurityContext.hpp"
#include "core/audit/IWSAPICall.hpp"
#include "core/auth/Auth.hpp"
#include "modules/websock-api/Messages.hpp"
#include "tools/AssertCast.hpp"
#include "tools/Conversion.hpp"
#include "tools/Mail.hpp"
#include "tools/db/database.hpp"
#include "tools/registry/GlobalRegistry.hpp"
#include <curl/curl.h>
#include <json.hpp>

using namespace Leosac;
using namespace Leosac::Module::SMTP;

SMTP::SMTP(zmqpp::context &ctx, zmqpp::socket *pipe,
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
    reactor_.add(bus_sub_, std::bind(&SMTP::handle_msg_bus, this));
}

SMTP::~SMTP()
{
    curl_global_cleanup();
}

void SMTP::handle_msg_bus()
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

void SMTP::process_config()
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
            server.url_  = itr.second.get<std::string>("url");
            server.from_ = itr.second.get<std::string>("from", "leosac@islog.com");
            server.verify_host_  = itr.second.get<bool>("verify_host", true);
            server.verify_peer_  = itr.second.get<bool>("verify_peer", true);
            server.CA_info_file_ = itr.second.get<std::string>("ca_file", "");

            INFO("SMTP module server: "
                 << Colorize::green(server.url_)
                 << ", verify_host: " << Colorize::green(server.verify_host_)
                 << ", verify_peer: " << Colorize::green(server.verify_peer_)
                 << ", ca_info: " << Colorize::green(server.CA_info_file_) << ")");
            smtp_config_->server_add(server);
        }
    }

    if (use_database_)
    {
        // Register a websocket handler.
        websocket_endpoint_ =
            std::make_unique<zmqpp::socket>(ctx_, zmqpp::socket_type::dealer);
        websocket_endpoint_->connect("inproc://SERVICE.WEBSOCKET");
        websocket_endpoint_->send(zmqpp::message() << "REGISTER_HANDLER"
                                                   << "module.smtp.getconfig");
        reactor_.add(*websocket_endpoint_.get(),
                     std::bind(&SMTP::handle_websocket_message, this));

        dispatcher_.register_handler("module.smtp.getconfig",
                                     std::bind(&SMTP::handle_ws_smtp_getconfig, this,
                                               std::placeholders::_1,
                                               std::placeholders::_2));
    }
}

void SMTP::prepare_curl(const MailInfo &mail)
{
    if (smtp_config_->servers().size() == 0)
        WARN("Cannot send mail titled " << Colorize::cyan(mail.title)
                                        << ". No SMTP server configured.");
    for (const auto &target : smtp_config_->servers())
    {
        auto curl = curl_easy_init();
        if (curl)
        {
            if (!target.CA_info_file_.empty())
                curl_easy_setopt(curl, CURLOPT_CAINFO, target.CA_info_file_.c_str());
            if (!target.verify_host_)
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            if (!target.verify_peer_)
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            if (target.username_.size())
                curl_easy_setopt(curl, CURLOPT_USERNAME, target.username_.c_str());
            if (target.password_.size())
                curl_easy_setopt(curl, CURLOPT_PASSWORD, target.password_.c_str());
            if (target.from_.size())
                curl_easy_setopt(curl, CURLOPT_MAIL_FROM, target.from_.c_str());

            ASSERT_LOG(target.url_.size(), "No mail server url.");
            curl_easy_setopt(curl, CURLOPT_URL, target.url_.c_str());

            send_mail(curl, mail);
            curl_easy_cleanup(curl);
        }
        else
        {
            ERROR("Cannot initialize curl_easy.");
        }
    }
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
    ss << "Date: " << Conversion<std::string>(std::chrono::system_clock::now())
       << "\r\n";
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

void SMTP::send_mail(CURL *curl, const MailInfo &mail)
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
    }
    else
    {
        INFO("Mail titled " << Colorize::cyan(mail.title) << " has been sent.");
    }
}

void SMTP::setup_database()
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
        srv.url_      = "smtp://mail.leosac.com";
        srv.username_ = "leosac-mail";
        srv.from_     = "leosac@leosac.com";
        cfg.server_add(srv);
        db->persist(cfg);

        t.commit();
    }
}

void SMTP::handle_websocket_message()
{
    zmqpp::message msg;
    websocket_endpoint_->receive(msg);

    if (msg.parts() == 1)
    {
        // Probably an response to REGISTER_HANDLER. Should work, otherwise
        // we abort.
        std::string tmp;
        msg >> tmp;
        ASSERT_LOG(tmp == "OK", "Something failed.");
        return;
    }

    ASSERT_LOG(msg.parts() == 3, "Ill formed message.");
    Audit::AuditEntryId audit_id;
    std::string connection_identifier;
    std::string content;
    msg >> audit_id >> connection_identifier >> content;

    json ws_request = json::parse(content);
    WebSockAPI::ClientMessage ws_msg;

    // Extract general message argument. This is sent by the WebSockAPI module
    // so we assume it is correct.
    ws_msg.uuid    = ws_request.at("uuid");
    ws_msg.type    = ws_request.at("type");
    ws_msg.content = ws_request.at("content");

    odb::session odb_session;

    WebSockAPI::ModuleRequestContext ctx;
    ctx.dbsrv = std::make_shared<DBService>(utils_->database());


    auto wsapi_call_audit = assert_cast<Audit::IWSAPICallPtr>(
        ctx.dbsrv->find_audit_by_id(audit_id, DBService::THROW_IF_NOT_FOUND));
    ctx.audit = wsapi_call_audit;
    UserSecurityContext security_ctx(ctx.dbsrv, wsapi_call_audit->author_id());

    // hardcoded for testing purpose

    if (security_ctx.check_permission(SecurityContext::Action::SMTP_GETCONFIG, {}))
    {

        WebSockAPI::ServerMessage ret = dispatcher_.dispatch(ctx, ws_msg);
        {
            odb::transaction t(utils_->database()->begin());
            wsapi_call_audit->status_code(ret.status_code);
            wsapi_call_audit->status_code(ret.status_code);
            wsapi_call_audit->status_string(ret.status_string);
            wsapi_call_audit->response_content(ret.content.dump(4));
            wsapi_call_audit->finalize();
            t.commit();
        }

        zmqpp::message response;
        response << "SEND_MESSAGE" << connection_identifier
                 << dispatcher_.convert_response(ret);
        websocket_endpoint_->send(response);
    }
    else
    {
        zmqpp::message response;
        response << "SEND_MESSAGE" << connection_identifier << "SORRY, NO";
        websocket_endpoint_->send(response);
    }
}

json SMTP::handle_ws_smtp_getconfig(const WebSockAPI::ModuleRequestContext &,
                                    const json &)
{
    json ret = json::array();
    for (const auto &server_cfg : smtp_config_->servers())
    {
        json server_desc;
        server_desc["url"]         = server_cfg.url_;
        server_desc["from"]        = server_cfg.from_;
        server_desc["username"]    = server_cfg.username_;
        server_desc["verify_host"] = server_cfg.verify_host_;
        server_desc["verify_peer"] = server_cfg.verify_peer_;
        ret.push_back(server_desc);
    }
    return ret;
}

Module::WebSockAPI::ServerMessage
RequestDispatcher::dispatch(const WebSockAPI::ModuleRequestContext &ctx,
                            const Module::WebSockAPI::ClientMessage &msg)
{
    auto handler = handlers_.find(msg.type);
    ASSERT_LOG(handler != handlers_.end(), "No handler found for " << msg.type);

    WebSockAPI::ServerMessage ret;
    ret.uuid        = msg.uuid;
    ret.type        = msg.type;
    ret.content     = handler->second(ctx, msg.content);
    ret.status_code = APIStatusCode::SUCCESS;
    return ret;
}

std::string
RequestDispatcher::convert_response(const Module::WebSockAPI::ServerMessage &msg)
{
    json json_message;

    json_message["uuid"]          = msg.uuid;
    json_message["type"]          = msg.type;
    json_message["status_code"]   = static_cast<int64_t>(msg.status_code);
    json_message["status_string"] = msg.status_string;
    json_message["content"]       = msg.content;

    return json_message.dump(4);
}
