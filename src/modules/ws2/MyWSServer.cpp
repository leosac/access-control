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

#include "modules/ws2/MyWSServer.hpp"
#include "modules/ws2/Parser.hpp"
#include "api/Common.hpp"
#include "core/CoreUtils.hpp"
#include "core/SecurityContext.hpp"
#include "core/audit/AuditFactory.hpp"
#include "core/audit/IWSAPICall.hpp"
#include "modules/ws2/ConnectionMetadata.hpp"
#include "modules/ws2/ExceptionConverter.hpp"
#include "modules/ws2/Exceptions.hpp"
#include "tools/db/DBService.hpp"
#include "tools/log.hpp"
#include <boost/asio/spawn.hpp>
#include <future>
#include "tools/enforce.hpp"

namespace Leosac
{
namespace Module
{
namespace WS2
{

ReqCtx::~ReqCtx()
{
}

MyWSServer::MyWSServer(const std::string &interface, uint16_t port,
                       CoreUtilsPtr utils)
    : handler_manager_(*this)
    , dbsrv_(std::make_shared<DBService>(utils->database()))
{
    // Websocket server configuration
    srv_.init_asio(&io_service_);

    boost::asio::ip::tcp::endpoint endpoint(
        boost::asio::ip::address::from_string(interface), port);

    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;

    // Register WS events handlers.
    srv_.set_open_handler(std::bind(&MyWSServer::on_open, this, _1));
    srv_.set_close_handler(std::bind(&MyWSServer::on_close, this, _1));
    srv_.set_message_handler(std::bind(&MyWSServer::on_message, this, _1, _2));
    srv_.set_reuse_addr(true);
    srv_.set_listen_backlog(5000);

    // Start network
    srv_.listen(endpoint);
    srv_.start_accept();

    for (int i = 0; i < nb_worked_thread; ++i)
    {
        worker_thread_.push_back(
            std::make_unique<std::thread>([this]() { io_service_.run(); }));
    }

    // HandlerManager::CoroutineHandlerInfo hi;
    HandlerManager::CoroutineHandlerInfo hi;
    hi.handler_                 = &API::get_leosac_version_coro;
    hi.force_processing_policy_ = true;
    hi.mpp_                     = MessageProcessingPolicy::QUEUED;

    handler_manager_.register_coroutine_handler("get_leosac_version", hi);
   // handler_manager_.register_coroutine_handler("get_leosac_version", &API::get_leosac_version_coro);
}

MyWSServer::~MyWSServer()
{
    start_shutdown();

    for (auto &thread_uptr : worker_thread_)
    {
        thread_uptr->join();
    }
}

CoreUtilsPtr MyWSServer::core_utils()
{
    return nullptr;
}

void MyWSServer::start_shutdown()
{
    srv_.get_io_service().post([this]() {
        srv_.stop_listening();

        {
            std::lock_guard<std::mutex> lg(metadata_mutex_);
            for (auto con_to_meta : metadatas_)
            {
                websocketpp::lib::error_code ec;
                srv_.close(con_to_meta.first, 0, "bye", ec);
                if (ec.value() == websocketpp::error::value::invalid_state)
                {
                    // Maybe the connection is already dead at this point.
                    // We can then safely ignore this error code.
                    continue;
                }
                ASSERT_LOG(ec.value() == 0,
                           BUILD_STR("Websocketpp error: " << ec.message()));
            }
        }
    });
}

void MyWSServer::on_open(websocketpp::connection_hdl hdl)
{
    {
        std::lock_guard<std::mutex> lg(metadata_mutex_);
        auto metadata                = std::make_shared<ConnectionMetadata>();
        metadata->connection_strand_ = srv_.get_con_from_hdl(hdl)->get_strand();
        metadatas_[hdl]              = metadata;
    }
    auto path = srv_.get_con_from_hdl(hdl)->get_resource();

    DEBUG("NEW CONNECTION: " << path);
}

void MyWSServer::on_close(websocketpp::connection_hdl hdl)
{
    auto md = metadata_from_connection_hdl(hdl);
    {
        std::lock_guard<std::mutex> lg(metadata_mutex_);
        metadatas_.erase(hdl);

        DEBUG("CONNECTION CLOSED. Handled " << md->request_count_ << " requests.");
    }
}

void MyWSServer::on_message(websocketpp::connection_hdl hdl,
                            WebsocketPPServer::message_ptr msg)
{
    ASSERT_LOG(srv_.get_con_from_hdl(hdl)->get_strand(), "NO_STRAND");
    const auto &strand = *srv_.get_con_from_hdl(hdl)->get_strand();
    auto metadata      = metadata_from_connection_hdl(hdl);
    // srv_.send(hdl, msg->get_payload(), websocketpp::frame::opcode::text);

    Audit::IWSAPICallPtr audit;

    ClientMessage client_message;
    try
    {
        if (audit_all)
        {
            audit = Audit::Factory::WSAPICall(dbsrv_->db());
        }

        // Parse the message.
        Parser p;
        client_message = p.parse(msg);
    }
    catch (const std::exception &ex)
    {
        ExceptionConverter ec;
        send_server_message(hdl, ec.create_response_from_error(
                                     std::current_exception(), client_message));
        return;
    }
    metadata->request_count_++;
    // Now that we know the message is valid-json and has been parsed,
    // we can continue. We'll build a ReqCtx.
    ReqCtx request_context(io_service_);
    request_context.audit_          = audit; // May be null.
    request_context.metadata_       = metadata;
    request_context.sc_             = request_context.metadata_->security_;
    request_context.connection_hdl_ = hdl;

    auto policy = determine_processing_policy(metadata, client_message);
    ProcessMessage(hdl, client_message, policy, false);
}

ConnectionMetadataPtr
MyWSServer::metadata_from_connection_hdl(websocketpp::connection_hdl hdl)
{
    std::lock_guard<std::mutex> lg(metadata_mutex_);
    LEOSAC_ENFORCE(metadatas_.count(hdl), "No metadata for connection. Connection is probably dead.");
    return metadatas_.at(hdl);
}

MessageProcessingPolicy
MyWSServer::determine_processing_policy(const ConnectionMetadataPtr &md,
                                        const ClientMessage &in) const
{
    MessageProcessingPolicy policy;
    if (handler_manager_.get_forced_processing_policy(in.type, policy))
    {
        return policy;
    }
    return md->processing_policy;
}

void MyWSServer::send_server_message(websocketpp::connection_hdl hdl,
                                     const ServerMessage &msg)
{
    json json_message;

    json_message["uuid"]          = msg.uuid;
    json_message["type"]          = msg.type;
    json_message["status_code"]   = static_cast<int64_t>(msg.status_code);
    json_message["status_string"] = msg.status_string;
    json_message["content"]       = msg.content;

    websocketpp::lib::error_code ec;
    srv_.send(hdl, json_message.dump(4), websocketpp::frame::opcode::text, ec);

    if (ec)
    {
        WARN("Failed to send message to client: " << ec.message());
    }
}

void MyWSServer::ProcessMessage(websocketpp::connection_hdl hdl,
                                const ClientMessage &msg,
                                MessageProcessingPolicy policy, bool serial_next)
{
    try
    {
        const auto &metadata = metadata_from_connection_hdl(hdl);
        ReqCtx request_context(io_service_);
        request_context.audit_          = nullptr;
        request_context.metadata_       = metadata;
        request_context.sc_             = metadata->security_;
        request_context.connection_hdl_ = hdl;

        HandlerType ht = handler_manager_.get_handler_type(msg.type);
        if (policy == MessageProcessingPolicy::PARALLEL)
        {
            // Clear metadata -- not thread safe.
            request_context.metadata_ = nullptr;
            if (ht == HandlerType::FUNCTION)
                handler_manager_.InvokeParallelFunctionHandler(request_context, msg);
            else
                handler_manager_.InvokeParallelCoroutineHandler(request_context,
                                                                msg);
        }
        else if (policy == MessageProcessingPolicy::CONCURRENT)
        {
            if (ht == HandlerType::FUNCTION)
                handler_manager_.InvokeConcurrentFunctionHandler(request_context,
                                                                 msg);
            else
                handler_manager_.InvokeConcurrentCoroutineHandler(request_context,
                                                                  msg);
        }
        else if (policy == MessageProcessingPolicy::QUEUED)
        {
            if (serial_next)
            {
                // We were called explicitly to handle next in queue message.
                // Therefore we MUST NOT be busy and we MUST NOT put the message back
                // into the queue.
                ASSERT_LOG(!metadata->busy_with_queued_msg(),
                           "Cannot be busy at the point.");
            }
            else if (metadata->has_pending_messages() ||
                     metadata->busy_with_queued_msg())
            {
                // If the connection's already busy processing a queued message,
                // we enqueue the message onto the connection's metadata's queue.
                DEBUG("QUEUEING MESSAGE FOR CONNECTION");
                metadata->messages_.push(msg);
                return;
            }
            if (ht == HandlerType::FUNCTION)
                handler_manager_.InvokeQueuedFunctionHandler(request_context, msg);
            else
                handler_manager_.InvokeQueuedCoroutineHandler(request_context, msg);
        }
    }
    catch (const std::exception &e)
    {
        ERROR("An exception occurred and was caught in ProcessMessage. "
              << e.what());
    }
}

void MyWSServer::process_next_queued_msg(ReqCtx rctx)
{
    const auto &md = rctx.metadata_;

    md->connection_strand_->post([this, md, rctx]() {
        // Process next queue message.
        DEBUG("WILL CHECK FOR PENDING MESSAGE");
        if (md->has_pending_messages())
        {
            DEBUG("FOUND SOME (BUSY ? " << md->busy_with_queued_msg());
            ClientMessage next_message = md->dequeue();
            ProcessMessage(rctx.connection_hdl_, next_message,
                           MessageProcessingPolicy::QUEUED, true);
        }
    });
}


HandlerManager::HandlerManager(MyWSServer &s)
    : server_(s)
{
}

bool HandlerManager::register_handler(const std::string &message_type,
                                      HandlerManager::HandlerInfo hi)
{
    std::lock_guard<decltype(mutex_)> lg(mutex_);

    if (has_handler(message_type))
        return false;

    handlers_[message_type] = hi;
    return true;
}

bool HandlerManager::has_handler(const std::string &message_type) const
{
    return handlers_.count(message_type) || coroutine_handlers_.count(message_type);
}

bool HandlerManager::register_coroutine_handler(
    const std::string &message_type, HandlerManager::CoroutineHandlerInfo chi)
{
    std::lock_guard<decltype(mutex_)> lg(mutex_);

    if (has_handler(message_type))
        return false;

    coroutine_handlers_[message_type] = chi;
    return true;
}

HandlerType HandlerManager::get_handler_type(const std::string &message_type) const
{
    std::lock_guard<decltype(mutex_)> lg(mutex_);
    LEOSAC_ENFORCE(
        has_handler(message_type),
        fmt::format("No handler for message with type '{}'", message_type));

    if (handlers_.count(message_type))
    {
        ASSERT_LOG(coroutine_handlers_.count(message_type) == 0,
                   "Cannot have multiple handler for a single type.");
        return HandlerType::FUNCTION;
    }
    if (coroutine_handlers_.count(message_type))
    {
        ASSERT_LOG(handlers_.count(message_type) == 0,
                   "Cannot have multiple handler for a single type.");
        return HandlerType::COROUTINE;
    }
    ASSERT_LOG(0, "Cannot be here.");
}

bool HandlerManager::get_forced_processing_policy(
    const std::string &message_type, MessageProcessingPolicy &out_policy) const
{
    std::lock_guard<decltype(mutex_)> lg(mutex_);
    LEOSAC_ENFORCE(
        has_handler(message_type),
        fmt::format("No handler for message with type '{}'", message_type));

    {
        // Search "normal" handlers.
        auto itr = handlers_.find(message_type);
        if (itr != handlers_.end())
        {
            if (itr->second.force_processing_policy_)
            {
                out_policy = itr->second.mpp_;
                return true;
            }
        }
    }
    {
        // Search coroutine backed handlers.
        auto itr = coroutine_handlers_.find(message_type);
        if (itr != coroutine_handlers_.end())
        {
            if (itr->second.force_processing_policy_)
            {
                out_policy = itr->second.mpp_;
                return true;
            }
        }
    }
    return false;
}

void HandlerManager::InvokeParallelFunctionHandler(ReqCtx rctx,
                                                   const ClientMessage &msg)
{
    std::lock_guard<decltype(mutex_)> lg(mutex_);
    ASSERT_LOG(rctx.metadata_ == nullptr,
               "Metadata should be null because they are not thread safe.");
    LEOSAC_ENFORCE(has_handler(msg.type), "No handler.");
    LEOSAC_ENFORCE(HandlerType::FUNCTION == get_handler_type(msg.type),
                   "Wrong handler type.");

    auto itr = handlers_.find(msg.type);
    LEOSAC_ENFORCE(itr != handlers_.end(), "Cannot find handler");

    HandlerInfo hi = itr->second;

    server_.io_service_.post([ this, rctx, msg, handler(hi.handler_) ]() mutable {
        try
        {
            ServerMessage smsg(msg);
            // Block the thread until the handler has completed.
            boost::optional<json> ret = handler(msg.content, rctx);
            if (ret)
            {
                smsg.content = *ret;
                server_.send_server_message(rctx.connection_hdl_, smsg);
            }
        }
        catch (const std::exception &ex)
        {
            ExceptionConverter ec;
            server_.send_server_message(
                rctx.connection_hdl_,
                ec.create_response_from_error(std::current_exception(), msg));
            return;
        }

    });
}


void HandlerManager::InvokeParallelCoroutineHandler(ReqCtx rctx,
                                                    const ClientMessage &msg)
{
    std::lock_guard<decltype(mutex_)> lg(mutex_);
    ASSERT_LOG(rctx.metadata_ == nullptr,
               "Metadata should be null because they are not thread safe.");
    LEOSAC_ENFORCE(has_handler(msg.type), "No handler.");
    LEOSAC_ENFORCE(HandlerType::COROUTINE == get_handler_type(msg.type),
                   "Wrong handler type.");

    auto itr = coroutine_handlers_.find(msg.type);
    LEOSAC_ENFORCE(itr != coroutine_handlers_.end(), "Cannot find handler");
    CoroutineHandlerInfo hi = itr->second;


    boost::asio::spawn(server_.io_service_, [
        this, rctx, msg, handler(hi.handler_)
    ](boost::asio::yield_context yc) mutable {
        try
        {
            ServerMessage smsg(msg);
            // Block the thread until the handler has completed.
            boost::optional<json> ret = handler(msg.content, rctx, yc);
            if (ret)
            {
                smsg.content = *ret;
                server_.send_server_message(rctx.connection_hdl_, smsg);
            }
        }
        catch (const std::exception &ex)
        {
            ExceptionConverter ec;
            server_.send_server_message(
                rctx.connection_hdl_,
                ec.create_response_from_error(std::current_exception(), msg));
            return;
        }

    });
}

void HandlerManager::InvokeConcurrentCoroutineHandler(ReqCtx rctx,
                                                      const ClientMessage &msg)
{
    std::lock_guard<decltype(mutex_)> lg(mutex_);
    LEOSAC_ENFORCE(has_handler(msg.type), "No handler.");
    LEOSAC_ENFORCE(HandlerType::COROUTINE == get_handler_type(msg.type),
                   "Wrong handler type.");

    auto itr = coroutine_handlers_.find(msg.type);
    LEOSAC_ENFORCE(itr != coroutine_handlers_.end(), "Cannot find handler");
    CoroutineHandlerInfo hi = itr->second;

    boost::asio::spawn(*rctx.metadata_->connection_strand_, [
        this, rctx, msg, handler(hi.handler_)
    ](boost::asio::yield_context yc) mutable {
        try
        {
            ServerMessage smsg(msg);
            // Block the thread until the handler has completed.
            boost::optional<json> ret = handler(msg.content, rctx, yc);
            if (ret)
            {
                smsg.content = *ret;
                server_.send_server_message(rctx.connection_hdl_, smsg);
            }
        }
        catch (const std::exception &ex)
        {
            ExceptionConverter ec;
            server_.send_server_message(
                rctx.connection_hdl_,
                ec.create_response_from_error(std::current_exception(), msg));
            return;
        }

    });
}

void HandlerManager::InvokeConcurrentFunctionHandler(ReqCtx rctx,
                                                     const ClientMessage &msg)
{
    std::lock_guard<decltype(mutex_)> lg(mutex_);
    LEOSAC_ENFORCE(has_handler(msg.type), "No handler.");
    LEOSAC_ENFORCE(HandlerType::FUNCTION == get_handler_type(msg.type),
                   "Wrong handler type.");

    auto itr = handlers_.find(msg.type);
    LEOSAC_ENFORCE(itr != handlers_.end(), "Cannot find handler");

    HandlerInfo hi = itr->second;

    rctx.metadata_->connection_strand_->post(
        [ this, rctx, msg, handler(hi.handler_) ]() mutable {
            try
            {
                ServerMessage smsg(msg);
                // Block the thread until the handler has completed.
                boost::optional<json> ret = handler(msg.content, rctx);
                if (ret)
                {
                    smsg.content = *ret;
                    server_.send_server_message(rctx.connection_hdl_, smsg);
                }
            }
            catch (const std::exception &ex)
            {
                ExceptionConverter ec;
                server_.send_server_message(
                    rctx.connection_hdl_,
                    ec.create_response_from_error(std::current_exception(), msg));
                return;
            }
        });
}

void HandlerManager::InvokeQueuedFunctionHandler(ReqCtx rctx,
                                                 const ClientMessage &msg)
{
    DEBUG("InvokeQueuedFunctionHandler");
    std::lock_guard<decltype(mutex_)> lg(mutex_);
    LEOSAC_ENFORCE(has_handler(msg.type), "No handler.");
    LEOSAC_ENFORCE(HandlerType::FUNCTION == get_handler_type(msg.type),
                   "Wrong handler type.");

    auto itr = handlers_.find(msg.type);
    LEOSAC_ENFORCE(itr != handlers_.end(), "Cannot find handler");

    HandlerInfo hi = itr->second;

    ASSERT_LOG(!rctx.metadata_->busy_with_queued_msg(), "Connection already busy");
    rctx.metadata_->busy_handling_queued_message_ = true;
    rctx.metadata_->connection_strand_->post(
        [ this, rctx, msg, handler(hi.handler_) ]() mutable {
            try
            {
                ServerMessage smsg(msg);
                // Block the thread until the handler has completed.
                boost::optional<json> ret = handler(msg.content, rctx);
                if (ret)
                {
                    smsg.content = *ret;
                    server_.send_server_message(rctx.connection_hdl_, smsg);
                }
            }
            catch (const std::exception &ex)
            {
                ExceptionConverter ec;
                server_.send_server_message(
                    rctx.connection_hdl_,
                    ec.create_response_from_error(std::current_exception(), msg));
                return;
            }
            // Done processing current message.
            // While we're holding the strand, we'll mark the connection as not
            // busy, dequeue the next message, and call ProcessMessage.
            auto md                           = rctx.metadata_;
            md->busy_handling_queued_message_ = false;
            server_.process_next_queued_msg(rctx);
        });
}

void HandlerManager::InvokeQueuedCoroutineHandler(ReqCtx rctx,
                                                  const ClientMessage &msg)
{
    DEBUG("InvokeQueuedCoroutineHandler");
    std::lock_guard<decltype(mutex_)> lg(mutex_);
    LEOSAC_ENFORCE(has_handler(msg.type), "No handler.");
    LEOSAC_ENFORCE(HandlerType::COROUTINE == get_handler_type(msg.type),
                   "Wrong handler type.");

    auto itr = coroutine_handlers_.find(msg.type);
    LEOSAC_ENFORCE(itr != coroutine_handlers_.end(), "Cannot find handler");
    CoroutineHandlerInfo hi = itr->second;

    ASSERT_LOG(!rctx.metadata_->busy_with_queued_msg(), "Connection already busy");
    rctx.metadata_->busy_handling_queued_message_ = true;

    boost::asio::spawn(*rctx.metadata_->connection_strand_, [
        this, rctx, msg, handler(hi.handler_)
    ](boost::asio::yield_context yc) mutable {
        try
        {
            ServerMessage smsg(msg);
            // Block the thread until the handler has completed.
            boost::optional<json> ret = handler(msg.content, rctx, yc);
            if (ret)
            {
                smsg.content = *ret;
                server_.send_server_message(rctx.connection_hdl_, smsg);
            }
        }
        catch (const std::exception &ex)
        {
            ExceptionConverter ec;
            server_.send_server_message(
                rctx.connection_hdl_,
                ec.create_response_from_error(std::current_exception(), msg));
            return;
        }

        // Done processing current message.
        // While we're holding the strand, we'll mark the connection as not
        // busy, dequeue the next message, and call ProcessMessage.
        auto md                           = rctx.metadata_;
        md->busy_handling_queued_message_ = false;
        server_.process_next_queued_msg(rctx);
    });
}
}
}
}
