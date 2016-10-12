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
#include "core/auth/Auth.hpp"
#include "tools/Conversion.hpp"
#include "tools/Mail.hpp"
#include "tools/db/database.hpp"
#include "tools/registry/GlobalRegistry.hpp"
#include <curl/curl.h>

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
        send_mail(mail);
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
}

void SMTP::send_mail(const MailInfo &mail)
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

            send_to_server(curl, target, mail);
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

void SMTP::send_to_server(CURL *curl, const SMTPServerInfo &srv,
                          const MailInfo &mail)
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
        t.commit();
    }
}
