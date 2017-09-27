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

#include "core/CoreUtils.hpp"
#include "core/GetServiceRegistry.hpp"
#include "modules/websock-api/Service.hpp"
#include "modules/websock-api/WebSockFwd.hpp"
#include "tools/AssertCast.hpp"
#include "tools/bs2.hpp"
#include "tools/service/ServiceRegistry.hpp"
#include <boost/asio.hpp>


namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
/**
 * This class provide a base implementation -designed to
 * be extended by subclass- so that non-boost-asio based module
 * can "easily" make use of the WebSockAPI module.
 *
 * The class provide extensions points that are called to
 * register and unregister websocket handler.
 * The function register_ws_handlers() is called when a
 * WSService is available. The user is responsible for
 * calling `unregister_ws_handler()`.
 *
 * @note This class is templated on a `Parameter` template that
 * allows the user to pass custom parameters to the support thread.
 *
 * @note A new thread is create in this class's constructor.
 */
template <typename Parameter>
class BaseModuleSupportThread
{
  public:
    explicit BaseModuleSupportThread(const CoreUtilsPtr &core_utils,
                                     const Parameter &param)
        : core_utils_(core_utils)
        , parameters_(param)
    {
        thread_ = std::make_unique<std::thread>([this]() { run_io_service(); });
    }

    virtual ~BaseModuleSupportThread()
    {
        work_ = nullptr;
        try
        {
            thread_->join();
        }
        catch (const std::exception &e)
        {
            ERROR("Failed to join WebSockAPI::BaseModuleSupportThread");
        }
    }

    void set_parameter(const Parameter &p)
    {
        std::lock_guard<decltype(mutex_)> lg(mutex_);
        parameters_ = p;
    }

    virtual void unregister_ws_handlers(Service &ws_service) = 0;

  private:
    void run_io_service()
    {
        auto ws_service = get_service_registry().get_service<WebSockAPI::Service>();
        service_event_listener_ = get_service_registry().register_event_listener(
            [this](const service_event::Event &e) { on_service_event(e); });

        // If we already have a WS Service object, register ws handler asap.
        if (ws_service)
        {
            io_.post([this, ws_service]() {
                std::lock_guard<decltype(mutex_)> lg(mutex_);
                register_ws_handlers(*ws_service);
            });
        }

        work_ = std::make_unique<boost::asio::io_service::work>(io_);
        io_.run();
    }

    void on_service_event(const service_event::Event &e)
    {
        // We can be called on any thread.
        if (e.type() == service_event::REGISTERED)
        {
            auto &service_registered_event =
                assert_cast<const service_event::ServiceRegistered &>(e);
            if (service_registered_event.interface_type() ==
                boost::typeindex::type_id<WebSockAPI::Service>())
            {
                // request registration of handler from our helper thread.
                io_.post([this]() {
                    auto ws_service =
                        get_service_registry().get_service<WebSockAPI::Service>();
                    ASSERT_LOG(ws_service, "WSService has disappeared");

                    {
                        std::lock_guard<decltype(mutex_)> lg(mutex_);
                        register_ws_handlers(*ws_service);
                    }
                });
            }
        }
    }

    /**
     * Called when websocket handler registration is possible.
     *
     * This function is called with the internal mutex locked,
     * to prevent conflict with a call to `set_parameters()`.
     */
    virtual void register_ws_handlers(Service &ws_service) = 0;

  protected:
    CoreUtilsPtr core_utils_;
    bs2::scoped_connection service_event_listener_;
    std::unique_ptr<std::thread> thread_;
    boost::asio::io_service io_;
    std::unique_ptr<boost::asio::io_service::work> work_;
    Parameter parameters_;
    std::mutex mutex_;
};
}
}
}
