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

#pragma once

#include "SMTPFwd.hpp"
#include "modules/BaseModule.hpp"
#include "modules/websock-api/RequestContext.hpp"
#include "modules/websock-api/WebSockFwd.hpp"
#include "tools/ToolsFwd.hpp"
#include <curl/curl.h>
#include <json.hpp>

namespace Leosac
{
namespace Module
{
namespace SMTP
{
using json = nlohmann::json;

class SMTPModule : public BaseModule
{
  public:
    SMTPModule(zmqpp::context &ctx, zmqpp::socket *pipe,
               const boost::property_tree::ptree &cfg, CoreUtilsPtr utils);

    ~SMTPModule();

  private:
    /**
     * Process a message that was read on the bus.
     */
    void handle_msg_bus();

    /**
     * A websocket message has been forwarded and dispatched to us.
     */
    WebSockAPI::ServerMessage
    handle_websocket_message(const WebSockAPI::ModuleRequestContext &request_ctx,
                             const WebSockAPI::ClientMessage &msg);

    /**
     * Process the websocket request "smtp.getconfig".
     */
    json handle_ws_smtp_getconfig(const WebSockAPI::ModuleRequestContext &,
                                  const json &);

    /**
     * Process the websocket request "smtp.setconfig".
     */
    json handle_ws_smtp_setconfig(const WebSockAPI::ModuleRequestContext &,
                                  const json &);

    /**
     * Process the websocket request "smtp.sendmail".
     */
    json handle_ws_smtp_sendmail(const WebSockAPI::ModuleRequestContext &,
                                 const json &);

    /**
     * Process the configuration file.
     */
    void process_config();

    /**
     * Read internal message bus.
     */
    zmqpp::socket bus_sub_;

    bool use_database_;

    /**
     * Configuration: either load from XML or database.
     */
    SMTPConfigUPtr smtp_config_;

    std::unique_ptr<WebSockAPI::Facade> websocket_api;

    void setup_database();

    bool prepare_curl(const MailInfo &mail);

    bool send_mail(CURL *curl, const MailInfo &mail);
    static constexpr const char *wshandler_getconfig = "module.smtp.getconfig";
    static constexpr const char *wshandler_setconfig = "module.smtp.setconfig";
    static constexpr const char *wshandler_sendmail  = "module.smtp.sendmail";
};
}
}
}
