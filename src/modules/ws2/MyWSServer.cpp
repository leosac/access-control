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
#include "api/Common.hpp"
#include "core/CoreUtils.hpp"
#include "core/SecurityContext.hpp"
#include "core/audit/AuditFactory.hpp"
#include "core/audit/IWSAPICall.hpp"
#include "modules/ws2/ConnectionMetadata.hpp"
#include "modules/ws2/ExceptionConverter.hpp"
#include "modules/ws2/Exceptions.hpp"
#include "modules/ws2/Parser.hpp"
#include "tools/ThreadUtils.hpp"
#include "tools/db/DBService.hpp"
#include "tools/enforce.hpp"
#include "tools/log.hpp"
#include <boost/asio/spawn.hpp>
#include <future>

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
        worker_thread_.push_back(std::make_unique<std::thread>([this]() {
            set_thread_name("mod_WS2_worker");
            io_service_.run();
        }));
    }

    handler_manager_.register_coroutine_handler("get_leosac_version",
                                                &API::get_leosac_version_coro,
                                                MessageProcessingPolicy::PARALLEL);
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
        auto metadata = std::make_shared<ConnectionMetadata>(
            srv_.get_con_from_hdl(hdl)->get_strand(), hdl);
        metadatas_[hdl] = metadata;
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

        DEBUG("CONNECTION CLOSED. Handled " << md->msg_count() << " requests.");
    }
}

void MyWSServer::on_message(websocketpp::connection_hdl hdl,
                            WebsocketPPServer::message_ptr msg)
{
    ASSERT_LOG(srv_.get_con_from_hdl(hdl)->get_strand(), "NO_STRAND");
    auto metadata = metadata_from_connection_hdl(hdl);
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
    metadata->incr_msg_count();
    // Now that we know the message is valid-json and has been parsed,
    // we can continue. We'll build a ReqCtx.
    ReqCtx request_context(io_service_);
    request_context.audit_          = audit; // May be null.
    request_context.metadata_       = metadata;
    request_context.sc_             = request_context.metadata_->security_context();
    request_context.connection_hdl_ = hdl;

    auto policy = determine_processing_policy(metadata, client_message);
    process_msg(hdl, client_message, policy, false);
}

ConnectionMetadataPtr
MyWSServer::metadata_from_connection_hdl(websocketpp::connection_hdl hdl)
{
    std::lock_guard<std::mutex> lg(metadata_mutex_);
    LEOSAC_ENFORCE(metadatas_.count(hdl),
                   "No metadata for connection. Connection is probably dead.");
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

void MyWSServer::process_msg(websocketpp::connection_hdl hdl,
                             const ClientMessage &msg,
                             MessageProcessingPolicy policy, bool serial_next)
{
    try
    {
        const auto &metadata = metadata_from_connection_hdl(hdl);
        ReqCtx request_context(io_service_);
        request_context.audit_          = nullptr;
        request_context.metadata_       = metadata;
        request_context.sc_             = metadata->security_context();
        request_context.connection_hdl_ = hdl;

        HandlerType ht = handler_manager_.get_handler_type(msg.type);
        if (policy == MessageProcessingPolicy::PARALLEL)
        {
            // Clear metadata -- not thread safe.
            request_context.metadata_ = nullptr;
            if (ht == HandlerType::FUNCTION)
                handler_manager_.invoke_parallel_fct_handler(request_context, msg);
            else
                handler_manager_.invoke_parallel_coro_handler(request_context, msg);
        }
        else if (policy == MessageProcessingPolicy::CONCURRENT)
        {
            if (ht == HandlerType::FUNCTION)
                handler_manager_.invoke_concurrent_fct_handler(request_context, msg);
            else
                handler_manager_.invoke_concurrent_coro_handler(request_context,
                                                                msg);
        }
        else if (policy == MessageProcessingPolicy::SERIAL)
        {
            if (serial_next)
            {
                // We were called explicitly to handle next in queue message.
                // Therefore we MUST NOT be busy and we MUST NOT put the message back
                // into the queue.
                ASSERT_LOG(!metadata->is_busy_for_serial(),
                           "Cannot be busy at the point.");
            }
            else if (metadata->has_pending_messages() ||
                     metadata->is_busy_for_serial())
            {
                // If the connection's already busy processing a queued message,
                // we enqueue the message onto the connection's metadata's queue.
                DEBUG("QUEUEING MESSAGE FOR CONNECTION");
                metadata->enqueue(msg);
                return;
            }
            if (ht == HandlerType::FUNCTION)
                handler_manager_.invoke_serial_fct_handler(request_context, msg);
            else
                handler_manager_.invoke_serial_coro_handler(request_context, msg);
        }
    }
    catch (const std::exception &e)
    {
        ERROR("An exception occurred and was caught in process_msg. " << e.what());
    }
}

void MyWSServer::process_next_serial_msg(const ConnectionMetadataPtr &md)
{
    md->mark_ready_for_serial();
    md->strand()->post([this, md]() {
        // Process next queue message.
        DEBUG("WILL CHECK FOR PENDING MESSAGE");
        if (md->has_pending_messages())
        {
            DEBUG("FOUND SOME (BUSY ? " << md->is_busy_for_serial());
            ClientMessage next_message = md->dequeue();
            process_msg(md->handle(), next_message, MessageProcessingPolicy::SERIAL,
                        true);
        }
    });
}


HandlerManager::HandlerManager(MyWSServer &s)
    : server_(s)
{
}

bool HandlerManager::has_handler(const std::string &message_type) const
{
    return handlers_.count(message_type) || coroutine_handlers_.count(message_type);
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
    ASSERT_CANNOT_BE_HERE(
        "Cannot find handler at a point we are garanteed to find one.");
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

void HandlerManager::invoke_parallel_fct_handler(ReqCtx rctx,
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

    server_.io_service_.post([ this, rctx, msg, handler(hi.handler_) ]() {
        do_invoke_fct_handler(rctx, msg, handler);
    });
}


void HandlerManager::invoke_parallel_coro_handler(ReqCtx rctx,
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

    boost::asio::spawn(
        server_.io_service_,
        [ this, rctx, msg, handler(hi.handler_) ](boost::asio::yield_context yc) {
            do_invoke_coro_handler(rctx, msg, handler, yc);
        });
}

void HandlerManager::invoke_concurrent_coro_handler(ReqCtx rctx,
                                                    const ClientMessage &msg)
{
    std::lock_guard<decltype(mutex_)> lg(mutex_);
    LEOSAC_ENFORCE(has_handler(msg.type), "No handler.");
    LEOSAC_ENFORCE(HandlerType::COROUTINE == get_handler_type(msg.type),
                   "Wrong handler type.");

    auto itr = coroutine_handlers_.find(msg.type);
    LEOSAC_ENFORCE(itr != coroutine_handlers_.end(), "Cannot find handler");
    CoroutineHandlerInfo hi = itr->second;

    boost::asio::spawn(
        *rctx.metadata_->strand(),
        [ this, rctx, msg, handler(hi.handler_) ](boost::asio::yield_context yc) {
            do_invoke_coro_handler(rctx, msg, handler, yc);
        });
}

void HandlerManager::invoke_concurrent_fct_handler(ReqCtx rctx,
                                                   const ClientMessage &msg)
{
    std::lock_guard<decltype(mutex_)> lg(mutex_);
    LEOSAC_ENFORCE(has_handler(msg.type), "No handler.");
    LEOSAC_ENFORCE(HandlerType::FUNCTION == get_handler_type(msg.type),
                   "Wrong handler type.");

    auto itr = handlers_.find(msg.type);
    LEOSAC_ENFORCE(itr != handlers_.end(), "Cannot find handler");

    HandlerInfo hi = itr->second;

    rctx.metadata_->strand()->post([ this, rctx, msg, handler(hi.handler_) ]() {
        do_invoke_fct_handler(rctx, msg, handler);
    });
}

void HandlerManager::invoke_serial_fct_handler(ReqCtx rctx, const ClientMessage &msg)
{
    std::lock_guard<decltype(mutex_)> lg(mutex_);
    LEOSAC_ENFORCE(has_handler(msg.type), "No handler.");
    LEOSAC_ENFORCE(HandlerType::FUNCTION == get_handler_type(msg.type),
                   "Wrong handler type.");

    auto itr = handlers_.find(msg.type);
    LEOSAC_ENFORCE(itr != handlers_.end(), "Cannot find handler");

    HandlerInfo hi = itr->second;

    ASSERT_LOG(!rctx.metadata_->is_busy_for_serial(), "Connection already busy");
    rctx.metadata_->mark_busy_for_serial();

    rctx.metadata_->strand()->post([ this, rctx, msg, handler(hi.handler_) ]() {
        do_invoke_fct_handler(rctx, msg, handler);
        server_.process_next_serial_msg(rctx.metadata_);
    });
}

void HandlerManager::invoke_serial_coro_handler(ReqCtx rctx,
                                                const ClientMessage &msg)
{
    std::lock_guard<decltype(mutex_)> lg(mutex_);
    LEOSAC_ENFORCE(has_handler(msg.type), "No handler.");
    LEOSAC_ENFORCE(HandlerType::COROUTINE == get_handler_type(msg.type),
                   "Wrong handler type.");

    auto itr = coroutine_handlers_.find(msg.type);
    LEOSAC_ENFORCE(itr != coroutine_handlers_.end(), "Cannot find handler");
    CoroutineHandlerInfo hi = itr->second;

    ASSERT_LOG(!rctx.metadata_->is_busy_for_serial(), "Connection already busy");
    rctx.metadata_->mark_busy_for_serial();

    boost::asio::spawn(
        *rctx.metadata_->strand(),
        [ this, rctx, msg, handler(hi.handler_) ](boost::asio::yield_context yc) {
            do_invoke_coro_handler(rctx, msg, handler, yc);
            server_.process_next_serial_msg(rctx.metadata_);
        });
}

void HandlerManager::do_invoke_coro_handler(ReqCtx rctx, const ClientMessage &msg,
                                            CoroutineRequestHandler handler,
                                            boost::asio::yield_context yc)
{
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
    }
}

void HandlerManager::do_invoke_fct_handler(ReqCtx rctx, const ClientMessage &msg,
                                           HandlerManager::RequestHandler handler)
{
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
    }
}
}
}
}
