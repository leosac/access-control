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

namespace Leosac
{
namespace Module
{
namespace WSNotifier
{
class WebServiceNotifier : public BaseModule
{
  public:
    WebServiceNotifier(zmqpp::context &ctx, zmqpp::socket *pipe,
                       const boost::property_tree::ptree &cfg, CoreUtilsPtr utils);

    ~WebServiceNotifier();

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
     * Send an HTTP request to the remote webservice
     * to let it know a card was read.
     */
    void send_card_info_to_remote(const std::string &card, int nb_bits);


    /**
     * Read internal message bus.
     */
    zmqpp::socket bus_sub_;

    /**
     * Some information for each webservice target.
     */
    struct TargetInfo
    {
        std::string url_;
        int connect_timeout_;
        int request_timeout_;
        /**
         * If SSL is enabled, do we perform certificate hostname validation ?
         */
        bool verify_host_;
        /**
         * If SSL is enabled, do we perform certificate validation ?
         */
        bool verify_peer_;
        /**
         * Path to a CA bundle file.
         */
        std::string CA_info_file_;
    };

    std::vector<TargetInfo> targets_;

    void send_to_target(void *curl, const Auth::WiegandCard &card,
                        const TargetInfo &target) noexcept;
};
}
}
}
