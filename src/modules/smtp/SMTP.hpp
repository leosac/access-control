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


class RequestDispatcher
{
  public:
    WebSockAPI::ServerMessage dispatch(const WebSockAPI::ModuleRequestContext &,
                                       const WebSockAPI::ClientMessage &msg);

    template <typename Callable>
    void register_handler(const std::string &type, Callable &&c)
    {
        handlers_[type] = c;
    }

    /**
     * Convert a ServerMessage to its string-JSON representation.
     */
    std::string convert_response(const WebSockAPI::ServerMessage &msg);

    std::map<
        std::string,
        std::function<json(const WebSockAPI::ModuleRequestContext &, const json &)>>
        handlers_;
};

class SMTP : public BaseModule
{
  public:
    SMTP(zmqpp::context &ctx, zmqpp::socket *pipe,
         const boost::property_tree::ptree &cfg, CoreUtilsPtr utils);

    ~SMTP();

  private:
    /**
     * Process a message that was read on the bus.
     */
    void handle_msg_bus();

    /**
     * A websocket message has been forwarded to us.
     */
    void handle_websocket_message();

    /**
     * Process the websocket request "smtp.getconfig".
     */
    json handle_ws_smtp_getconfig(const WebSockAPI::ModuleRequestContext &,
                                  const json &);

    /**
     * Process the configuration file.
     */
    void process_config();

    /**
     * Read internal message bus.
     */
    zmqpp::socket bus_sub_;

    /**
     * A DEALER socket.
     */
    std::unique_ptr<zmqpp::socket> websocket_endpoint_;

    bool use_database_;

    /**
     * Configuration: either load from XML or database.
     */
    SMTPConfigUPtr smtp_config_;

    RequestDispatcher dispatcher_;

    void setup_database();

    void prepare_curl(const MailInfo &mail);

    void send_mail(CURL *curl, const MailInfo &mail);
};
}
}
}
