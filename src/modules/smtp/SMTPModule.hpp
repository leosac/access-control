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

#pragma once

#include "SMTPFwd.hpp"
#include "core/audit/serializers/PolymorphicAuditSerializer.hpp"
#include "modules/AsioModule.hpp"
#include "modules/websock-api/RequestContext.hpp"
#include "modules/websock-api/WebSockFwd.hpp"
#include "tools/ToolsFwd.hpp"
#include "tools/bs2.hpp"
#include "tools/service/ServiceRegistry.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <curl/curl.h>
#include <json.hpp>

namespace Leosac
{
namespace Module
{
namespace SMTP
{
using json = nlohmann::json;

class SMTPModule : public AsioModule
{
  public:
    SMTPModule(zmqpp::context &ctx, zmqpp::socket *pipe,
               const boost::property_tree::ptree &cfg, CoreUtilsPtr utils);

    ~SMTPModule();

    /**
     * Asynchronously and thread-safely send an email.
     *
     * This method doesn't provide a way to inform the caller of
     * completion of his operation.
     */
    void async_send_mail(const MailInfo &mail);

  private:
    /**
     * Process the websocket request "smtp.getconfig".
     */
    json handle_ws_smtp_getconfig(const WebSockAPI::RequestContext &, const json &);

    /**
     * Process the websocket request "smtp.setconfig".
     */
    json handle_ws_smtp_setconfig(const WebSockAPI::RequestContext &, const json &);

    /**
     * Process thesocket request "smtp.sendmail".
     */
    json handle_ws_smtp_sendmail(const WebSockAPI::RequestContext &, const json &);

    /**
     * Process the configuration file.
     */
    void process_config();

    bool use_database_;

    /**
     * Configuration: either load from XML or database.
     */
    SMTPConfigUPtr smtp_config_;

    void setup_database();

    bool prepare_curl(const MailInfo &mail);

    bool send_mail(CURL *curl, const MailInfo &mail);
    static constexpr const char *wshandler_getconfig = "module.smtp.getconfig";
    static constexpr const char *wshandler_setconfig = "module.smtp.setconfig";
    static constexpr const char *wshandler_sendmail  = "module.smtp.sendmail";

    virtual void on_service_event(const service_event::Event &) override;

    /**
     * Attempt to register websocket handlers against the websocket service,
     * if available.
     *
     * If it cannot register handlers, this function fails silently (because
     * most of the time we don't care at the point the function is called).
     */
    void register_ws_handlers();
};
}
}
}
