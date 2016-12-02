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

#include "AsioModule.hpp"
#include <core/GetServiceRegistry.hpp>

namespace Leosac
{
namespace Module
{

AsioModule::AsioModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                       const boost::property_tree::ptree &cfg, CoreUtilsPtr utils)
    : BaseModule(ctx, pipe, cfg, utils)
{
    service_event_listener_ = get_service_registry().register_event_listener(
        [this](const service_event::Event &e) { on_service_event(e); });
}

AsioModule::~AsioModule()
{
    service_event_listener_.disconnect();
}

void AsioModule::run()
{
    work_ = std::make_unique<boost::asio::io_service::work>(io_service_);
    install_async_handlers();
    io_service_.run();
}

void AsioModule::install_async_handlers()
{
    auto stop_watcher(std::make_shared<StopWatcher>(*this));
    stop_watcher->schedule_wait();

    auto reactor_poller(std::make_shared<AsyncReactorPoller>(*this));
    reactor_poller->schedule_wait();
}

void AsioModule::StopWatcher::wait(const boost::system::error_code &ec)
{
    if (ec)
    {
        WARN("SMTPModule StopWatcher timer has errored: " << ec.message());
        ASSERT_LOG(0, "Failed.");
    }
    if (!self_.is_running_)
        self_.work_.reset();
    schedule_wait();
}

void AsioModule::StopWatcher::schedule_wait()
{
    timer_.expires_from_now(std::chrono::milliseconds(1000));
    timer_.async_wait(std::bind(&AsioModule::StopWatcher::wait, shared_from_this(),
                                std::placeholders::_1));
}

void AsioModule::AsyncReactorPoller::schedule_wait()
{
    timer_.expires_from_now(std::chrono::milliseconds(25));
    timer_.async_wait(std::bind(&AsioModule::AsyncReactorPoller::wait_handler,
                                shared_from_this(), std::placeholders::_1));
}

void AsioModule::AsyncReactorPoller::wait_handler(
    const boost::system::error_code &ec)
{
    ASSERT_LOG(ec == 0, "Error while processing wait_handler: " << ec.message());
    self_.reactor_.poll(0);
    schedule_wait();
}
}
}
