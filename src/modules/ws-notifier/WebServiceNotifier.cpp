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

#include "WebServiceNotifier.hpp"
#include "core/auth/Auth.hpp"
#include "core/credentials/RFIDCard.hpp"
#include <curl/curl.h>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WSNotifier;

WebServiceNotifier::WebServiceNotifier(zmqpp::context &ctx, zmqpp::socket *pipe,
                                       const boost::property_tree::ptree &cfg,
                                       CoreUtilsPtr utils)
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
    process_config();
    reactor_.add(bus_sub_, std::bind(&WebServiceNotifier::handle_msg_bus, this));
}

WebServiceNotifier::~WebServiceNotifier()
{
    curl_global_cleanup();
}

void WebServiceNotifier::handle_msg_bus()
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

    send_card_info_to_remote(src, card, bits);
}

void WebServiceNotifier::process_config()
{
    for (auto &&itr : config_.get_child("module_config.sources"))
    {
        auto name = itr.second.get<std::string>("");
        bus_sub_.subscribe("S_" + name);
    }

    for (auto &&itr : config_.get_child("module_config.targets"))
    {
        TargetInfo target;
        target.url_             = itr.second.get<std::string>("url");
        target.connect_timeout_ = itr.second.get<int>("connect_timeout", 7000);
        target.request_timeout_ = itr.second.get<int>("request_timeout", 7000);
        target.verify_host_     = itr.second.get<bool>("verify_host", true);
        target.verify_peer_     = itr.second.get<bool>("verify_peer", true);
        target.CA_info_file_    = itr.second.get<std::string>("ca_file", "");

        INFO("WS-Notifier remote target: "
             << Colorize::green(target.url_)
             << " (connect_timeout: " << Colorize::green(target.connect_timeout_)
             << ", request_timeout: " << Colorize::green(target.request_timeout_)
             << ", verify_host: " << Colorize::green(target.verify_host_)
             << ", verify_peer: " << Colorize::green(target.verify_peer_)
             << ", ca_info: " << Colorize::green(target.CA_info_file_) << ")");
        targets_.push_back(std::move(target));
    }
}

void WebServiceNotifier::send_card_info_to_remote(const std::string &auth_source,
                                                  const std::string &card_hex,
                                                  int nb_bits)
{
    auto card = Cred::RFIDCard();
    card.card_id(card_hex);
    card.nb_bits(nb_bits);

    for (const auto &target : targets_)
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
            send_to_target(curl, auth_source, card, target);
            curl_easy_cleanup(curl);
        }
        else
        {
            ERROR("Cannot initialize curl_easy.");
        }
    }
}

/**
 * We don't care about getting the resulting page from the webservice.
 *
 * We use this function as a callback for CURL.
 */
static size_t write_callback(char * /*ptr*/, size_t size, size_t nmemb,
                             void * /*userdata*/)
{
    return size * nmemb;
}

void WebServiceNotifier::send_to_target(
    void *curl, const std::string &auth_source, const Cred::RFIDCard &card,
    const WebServiceNotifier::TargetInfo &target) noexcept
{
    assert(curl);

    std::string post_fields =
        fmt::format("card_id={}&auth_source={}&card_id_raw={}", card.to_int(),
                    auth_source, card.to_raw_int());

    curl_easy_setopt(curl, CURLOPT_URL, target.url_.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields.c_str());

    // timeouts
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, target.connect_timeout_);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, target.request_timeout_);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, nullptr);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_callback);

    auto res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        WARN("curl_easy_perform() failed: " << curl_easy_strerror(res));
    }
}
