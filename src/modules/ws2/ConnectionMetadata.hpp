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

#include "LeosacFwd.hpp"
#include "MyWSServer.hpp"
#include "modules/ws2/Parser.hpp"
#include <queue>
#include <string>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

namespace Leosac
{
namespace Module
{
namespace WS2
{

/**
 * Metadata for a websocket connection.
 *
 * Each websocket connection has an associated
 * ConnectionMetadata object.
 */
struct ConnectionMetadata
{
    /**
     * Create a ConnectionMetadata, definitively associating an
     * ASIO's strand and a Websocketpp connection handle
     * to it.
     *
     * @param connection_strand The ASIO strand in which the connection's handler
     * executes. MUST NOT BE NULL.
     * @param connection_handle The Websocketpp handle to the connection.
     */
    ConnectionMetadata(std::shared_ptr<boost::asio::strand> connection_strand,
                       websocketpp::connection_hdl connection_handle);

    /**
     * Check if the connection associated with this metadata
     * object has pending messages (store in this metadata object)
     * that needs to be processed.
     *
     * This apply for message whose processing policy is SERIAL.
     */
    bool has_pending_messages() const;

    /**
     * Add `msg` to the queue of messages to be processed.
     */
    void enqueue(const ClientMessage &msg);

    /**
     * Retrieve the next queued message.
     *
     * You MUST make sure that such message exists. Call has_pending_messages()
     * before.
     */
    ClientMessage dequeue();

    /**
     * Retrieve the SecurityContext associated with this connection.
     *
     * This function will never return nullptr. If no context is
     * associated to the connection, it will return a fresh instance
     * of a NullSecurityContext.
     */
    SecurityContextCPtr security_context() const;

    /**
     * Are we busy handling any serial message at this point ?
     */
    bool is_busy_for_serial() const;

    /**
     * Mark the connection as ready to process a SERIAL message.
     *
     * This means that no handler with SERIAL policy are alive
     * at this time.
     */
    void mark_ready_for_serial();

    /**
     * Mark the connection as busy handling message with SERIAL
     * policy.
     */
    void mark_busy_for_serial();

    const MessageProcessingPolicy processing_policy{MessageProcessingPolicy::SERIAL};

    /**
     * Increment the count of message received on this connection.
     */
    void incr_msg_count();

    /**
     * Return the number of message received on this connection.
     */
    size_t msg_count() const;

    /**
     * Retrieve the strand associated to the connection.
     */
    const std::shared_ptr<boost::asio::strand> &strand() const;

    /**
     * Retrieve the connection handle of the connection this
     * metadata object represents.
     */
    const websocketpp::connection_hdl &handle() const;

  private:
    std::string uuid_;

    /**
     * A boolean to keep track of wether or not
     * we are currently handling a message with
     * SERIAL processing policy.
     */
    bool ready_for_serial_{true};

    /**
     * Total of message received by the connection.
     */
    size_t msg_count_{0};

    std::queue<ClientMessage> messages_;

    std::shared_ptr<boost::asio::strand> connection_strand_;

    websocketpp::connection_hdl connection_hdl_;

    SecurityContextCPtr security_;
};
}
}
}
