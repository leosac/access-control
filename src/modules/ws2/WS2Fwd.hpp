/*
    Copyright (C) 2014-2017 Leosac

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
#include "core/SecurityContext.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/optional.hpp>
#include <json.hpp>
#include <memory>
#include <websocketpp/common/connection_hdl.hpp>

namespace Leosac
{
namespace Module
{
namespace WS2
{
class ConnectionMetadata;
using ConnectionMetadataPtr = std::shared_ptr<ConnectionMetadata>;
class MyWSServer;
class HandlerManager;

class ClientMessage;

class ReqCtx
{
  public:
    ~ReqCtx();

    ReqCtx(boost::asio::io_service &io)
        : io_service_(io)
    {
    }

    ReqCtx(const ReqCtx &o)
        : audit_(o.audit_)
        , sc_(o.sc_)
        , metadata_(o.metadata_)
        , connection_hdl_(o.connection_hdl_)
        , io_service_(o.io_service_)
    {
    }

    Audit::IAuditEntryPtr audit_;
    SecurityContextCPtr sc_;

    ConnectionMetadataPtr metadata_; // may be null
    websocketpp::connection_hdl connection_hdl_;
    boost::asio::io_service &io_service_;
};


/**
* The available threading policies wrt message handle.
*
* Each message type can force a message policy onto
* its handler. If no policy is forced, we fallback
* on the connection-level processing policy (whose default
* is SERIAL).
*/
enum class MessageProcessingPolicy
{
    /**
     * Message are enqueued on the source connection's queue.
     * This means that no two coroutines or handler processing a message
     * with SERIAL policy for the connection can *exist* simultaneously.
     *
     * The coroutine or handler responsible for the message runs
     * on the connection's strand.
     *
     * Messages with this policy are therefore processed in the order
     * in which they are read from the socket.
     */
    SERIAL,

    /**
     * The message can be processed concurrently to other messages
     * with a CONCURRENT policy.
     *
     * It may be processed concurrently (but not in parallel) with a message
     * having a SERIAL processing policy.
     *
     * Handlers with CONCURRENT policy executes on the strand of the connection
     * from which the message originates. This means that no two CONCURRENT
     * handler (from a given connection) can *run* at the same time on multiple
     * threads.
     */
    CONCURRENT,

    /**
     * The PARALLEL policy doesn't impose additional constraints (such
     * as strand or queueing) to the messages that use it.
     *
     * The originating connection's metadatas ARE NOT available to
     * handlers running in PARALLEL mode. This is because ConnectionMetadata
     * objects are not thread-safe.
     *
     * The handler runs in any of the worker threads and DOES NOT
     * use a strand.
     */
    PARALLEL
};

/**
 * This enum describe the "type" of message handlers.
 *
 * Simply put, a message handler can either be a normal
 * callable (FUNCTION), or a coroutine.
 */
enum class HandlerType
{
    /**
     * Any normal callable object.
     */
    FUNCTION,

    /**
     * A callable that can behave like a coroutine.
     * COROUTINE handlers accept an additional parameter,
     * the yield context.
     */
    COROUTINE
};

using json = nlohmann::json;
}
}
}
