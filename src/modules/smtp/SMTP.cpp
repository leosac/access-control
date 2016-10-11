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
#include "core/auth/Auth.hpp"
#include "core/auth/WiegandCard.hpp"
#include "tools/Conversion.hpp"
#include "tools/MyTime.hpp"
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
    std::string src;
    Leosac::Auth::SourceType type;
    std::string card;
    int bits;

    bus_sub_.receive(msg);
    if (msg.parts() < 4)
    {
        WARN("Unexpected message content.");
        return;
    }
    msg >> src >> type >> card >> bits;
    if (type != Leosac::Auth::SourceType::SIMPLE_WIEGAND)
    {
        INFO("WS-Notifier cannot handle this type of credential yet.");
        return;
    }
}

void SMTP::process_config()
{
    for (auto &&itr : config_.get_child("module_config.targets"))
    {
        SMTPServerInfo server;
        server.url_ = itr.second.get<std::string>("url");
        // server.connect_timeout_ = itr.second.get<int>("connect_timeout", 7000);
        // server.request_timeout_ = itr.second.get<int>("request_timeout", 7000);
        server.verify_host_  = itr.second.get<bool>("verify_host", true);
        server.verify_peer_  = itr.second.get<bool>("verify_peer", true);
        server.CA_info_file_ = itr.second.get<std::string>("ca_file", "");

        INFO("SMTP module server: "
             << Colorize::green(server.url_)
             //<< " (connect_timeout: " << Colorize::green(server.connect_timeout_)
             //<< ", request_timeout: " << Colorize::green(server.request_timeout_)
             << ", verify_host: " << Colorize::green(server.verify_host_)
             << ", verify_peer: " << Colorize::green(server.verify_peer_)
             << ", ca_info: " << Colorize::green(server.CA_info_file_) << ")");
        servers_.push_back(std::move(server));
    }
}

void SMTP::send_mail(const MailInfo &mail)
{
    for (const auto &target : servers_)
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
                curl_easy_setopt(curl, CURLOPT_USERNAME, target.username_);
            if (target.password_.size())
                curl_easy_setopt(curl, CURLOPT_PASSWORD, target.password_);
            curl_easy_setopt(curl, CURLOPT_URL, target.url_);

            send_to_server(curl, target, mail);
            curl_easy_cleanup(curl);
        }
        else
        {
            ERROR("Cannot initialize curl_easy.");
        }
    }
}

void SMTP::send_to_server(CURL *curl, const SMTPServerInfo &srv,
                          const MailInfo &mail)
{
    assert(curl);
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, srv.from_);

    struct curl_slist *recipients = NULL;
    for (const auto &recipient : mail.to)
        recipients = curl_slist_append(recipients, recipient.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    struct UploadStatus
    {
        const MailInfo &mail;
        int counter;
    } status{.mail = mail, .counter = 0};

    auto readfct = [](void *ptr, size_t size, size_t nmemb, void *userp) {
        UploadStatus *st = static_cast<UploadStatus *>(userp);
        ASSERT_LOG(st, "UploadStatus is null.");

        auto content     = build_mail_str(st->mail);
        auto wanted      = size * nmemb;
        auto available   = content.size() - st->counter;
        auto to_transfer = std::min(available, wanted);

        std::memcpy(ptr, &content, to_transfer);
        st->counter += to_transfer;

        return to_transfer;
    };

    curl_easy_setopt(curl, CURLOPT_READFUNCTION, &readfct);
    curl_easy_setopt(curl, CURLOPT_READDATA, &status);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    // timeouts
    /*
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, target.connect_timeout_);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, target.request_timeout_);*/

    auto res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        WARN("curl_easy_perform() failed: " << curl_easy_strerror(res));
    }
}

std::string SMTP::build_mail_str(const MailInfo &mail)
{
    std::stringstream ss;

    ASSERT_LOG(mail.to.size(), "No recipients for mail.");
    ss << "Date: " << Conversion<std::string>(std::chrono::system_clock::now())
       << "\r\n";
    ss << "To: " << mail.to.at(0) << "\r\n";

    ss << "Subject: " << mail.title << "\r\n";
    ss << "\r\n"; // empty line to divide headers from body, see RFC5322
    ss << mail.body;

    return ss.str();
}
