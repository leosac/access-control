/*
    Copyright (C) 2014-2015 Islog

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

#include "modules/BaseModule.hpp"
#include "core/auth/AuthFwd.hpp"
#include "protocols/PushSimpleCardNumber.hpp"

namespace Leosac
{
  namespace Module
  {
    namespace TCPNotifier
    {
      class ProtocolHandler;
      using ProtocolHandlerUPtr = std::unique_ptr<ProtocolHandler>;

      class TCPNotifierModule : public BaseModule
      {
      public:
        TCPNotifierModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                          const boost::property_tree::ptree &cfg,
                          CoreUtilsPtr utils);

        ~TCPNotifierModule();

      private:
        /**
         * Process a message that was read on the bus.
         */
        void handle_msg_bus();

        /**
         * Some event on our ZMQ Stream socket.
         */
        void handle_tcp_msg();

        /**
         * Process the configuration file.
         */
        void process_config();

        /**
         * Send a TCP packet to every configured and active
         * tcp server.
         */
        void send_card_info_to_remote(const std::string &card, int nb_bits);

        /**
         * Read internal message bus.
         */
        zmqpp::socket bus_sub_;

        /**
        * Stream socket used to connect to remote client we want to
        * notify.
        */
        zmqpp::socket tcp_;

        /**
         * Some information for each tcp server target.
         */
        struct TargetInfo
        {
          // Url is IP:PORT
          std::string url_;

          // The ZMQ routing-id for this target.
          std::string zmq_identity_;

          // ZMQ provide auto reconnection
          // This tracks the status.
          bool status_;

          ProtocolHandlerUPtr protocol_;
        };

        std::vector<TargetInfo> targets_;
        TargetInfo &find_target(const std::string &routing_id);
      };
    }
  }
}
