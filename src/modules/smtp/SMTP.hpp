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

#include "core/auth/AuthFwd.hpp"
#include "modules/BaseModule.hpp"
#include "tools/Mail.hpp"
#include <curl/curl.h>

namespace Leosac
{
namespace Module
{
namespace SMTP
{
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
     * Process the configuration file.
     */
    void process_config();

    /**
     * Read internal message bus.
     */
    zmqpp::socket bus_sub_;

    /**
     * Some information for each webservice target.
     */
    struct SMTPServerInfo
    {
        std::string url_;

        std::string from_;

        std::string username_;

        std::string password_;

        bool verify_host_;
        bool verify_peer_;
        std::string CA_info_file_;
    };

    std::vector<SMTPServerInfo> servers_;
    static std::string build_mail_str(const MailInfo &mail);

    void send_mail(const MailInfo &mail);

    void send_to_server(CURL *curl, const SMTPServerInfo &srv, const MailInfo &mail);
};
}
}
}
