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

#include <zmqpp/socket.hpp>
#include <zmqpp/reactor.hpp>
#include "LeosacFwd.hpp"
#include "core/auth/AuthFwd.hpp"
#include "ProtocolHandler.hpp"

namespace Leosac
{
  namespace Module
  {
    namespace TCPNotifier
    {
      class NotifierInstance;
      using NotifierInstanceUPtr = std::unique_ptr<NotifierInstance>;

      /**
       * This is an instance of the Notifier. The module can have
       * many NotifierInstance running concurrently.
       *
       * Multiple instance are useful because they let the module
       * act as server and/or client and talk to various peer using
       * different protocols.
       *
       * A instance is qualified by its status (server or client) and
       * the protocol it speaks.
       *
       *
       * @note Implementation note about peer management.
       * When acting as a server, we remove a disconnected client from
       * our targets_ (list of peer), and we create a new target when a client
       * connects.
       * When acting as a client, we always keep the list of server we connect
       * to,
       * and manage the state (connected or not).
       */
      class NotifierInstance
      {
      public:
        /**
         * Create a new notifier instance.
         *
         * The context and the reactor are provided by the TcpNotifier module.
         * The NotifierInstance shall register its socket to the reactor.
         *
         * @warning The Notifier can either bind or connect but not both at the
         * same
         * time.
         *
         * @param ctx The ZeroMQ context.
         * @param reactor The reactor of the parent module. We use this to
         * register callback
         * on socket.
         * @param auth_source The list of authentication source to listen to.
         */
        NotifierInstance(zmqpp::context &ctx, zmqpp::reactor &reactor,
                         std::vector<std::string> auth_sources,
                         std::vector<std::string> connect_to,
                         std::vector<std::string> bind_to,
                         ProtocolHandlerUPtr protocol_handler);

        ~NotifierInstance() = default;

        NotifierInstance(const NotifierInstance &) = delete;
        NotifierInstance &operator=(const NotifierInstance &) = delete;
        NotifierInstance(NotifierInstance &&o) = delete;
        NotifierInstance &operator=(NotifierInstance &&o) = delete;

      private:
        /**
         * Notify the peers of the `card` credential.
         */
        void handle_credential(Auth::WiegandCard &card);

        void handle_one(zmqpp::message &msg);

        void handle_msg_bus();

        /**
         * Some event on our ZMQ Stream socket.
         */
        void handle_tcp_msg();

        void configure_tcp_socket(const std::vector<std::string> &endpoints);

        /**
         * Some information for each tcp server target.
         */
        struct TargetInfo
        {
          TargetInfo()
              : status_(false)
          {
          }
          TargetInfo(const TargetInfo &) = default;
          TargetInfo(TargetInfo &&o) = default;
          TargetInfo &operator=(const TargetInfo &o) = default;
          TargetInfo &operator=(TargetInfo &&o) = default;

          // Url is IP:PORT
          // Maybe be empty is peer is connecting to us.
          std::string url_;

          // The ZMQ routing-id for this target.
          std::string zmq_identity_;

          // ZMQ provide auto reconnection
          // This tracks the status.
          bool status_;
        };

        /**
         * Read internal message bus.
         */
        zmqpp::socket bus_sub_;

        /**
         * Stream socket used to connect to remote client we want to
         * notify.
         */
        zmqpp::socket tcp_;

        std::list<TargetInfo> targets_;

        ProtocolHandlerUPtr protocol_;

        /**
         * Are we a server or a client ?
         */
        bool act_as_server_;

        /**
         * Attempt to find a target from its routing_id.
         *
         * Returns nullptr if the target cannot be found.
         */
        TargetInfo *find_target(const std::string &routing_id);
      };
    }
  }
}
