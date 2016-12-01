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

#include "NotifdModule.hpp"
#include "core/GetServiceRegistry.hpp"
#include <boost/asio.hpp>

namespace Leosac
{

namespace Module
{
namespace Notifd
{
NotifdModule::NotifdModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                           const boost::property_tree::ptree &cfg,
                           CoreUtilsPtr utils)
    : BaseModule(ctx, pipe, cfg, utils)
{
}

NotifdModule::~NotifdModule()
{
    ASSERT_LOG(io_service_->stopped(), "io_service not stopped.");
}

void NotifdModule::run()
{
    io_service_ = std::make_shared<boost::asio::io_service>();

    install_stop_watcher();
    io_service_->run();
    DEBUG("DONE RUNNING !");
}

void NotifdModule::install_stop_watcher()
{
    // Not very good because it can introduces a delay of up to 1second.
    // However, there is a bug in libzmq that make watching the FD not working.
    // See https://github.com/zeromq/libzmq/issues/1434
    /*    boost::fibers::async(
            boost::fibers::launch::post, std::allocator_arg,
            ValgrindStackAllocator<void>::instance(), [this]() {
                while (true)
                {
                    zmqpp::message msg;
                    if (pipe_.receive(msg, true))
                    {
                        assert(msg.is_signal());
                        zmqpp::signal sig;
                        msg >> sig;
                        if (sig == zmqpp::signal::stop)
                        {
                            is_running_ = false;
                            FiberCounter::instance().wait_zero();
                            io_service_->post([this]() { io_service_->stop(); });
                            return;
                        }
                    }
                    else
                    {
                        DEBUG("NOTIFD SLEEPING A BIT...");
                        boost::this_fiber::sleep_for(std::chrono::seconds(1));
                    }
                }
            });*/
}
}
}
}
