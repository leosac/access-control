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

#include "BaseModule.hpp"
#include "tools/bs2.hpp"
#include "tools/service/ServiceRegistry.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>

namespace Leosac
{
namespace Module
{

/**
 * This is a base class for boost::asio 'aware' module.
 *
 * It builds on top of the BaseModule but the main loop
 * in provide by boost::asio::io_service.
 *
 * It periodically polls for signals / messages from the normal
 * messaging infrastructure.
 */
class AsioModule : public BaseModule
{
  public:
    AsioModule(zmqpp::context &ctx, zmqpp::socket *pipe,
               const boost::property_tree::ptree &cfg, CoreUtilsPtr utils);

    ~AsioModule();

    virtual void run() override final;

    /**
     * Post some work onto the work queue of the module.
     *
     * @note This function is thread-safe.
     * @note Callable `post()`ed through this function are
     * guaranteed to be executed in the module's thread.
     */
    template <typename Callable>
    void post(Callable &&callable)
    {
        io_service_.post(std::forward<Callable>(callable));
    }

  protected:
    boost::asio::io_service io_service_;

    /**
     * Function invoked when a service event is triggered.
     *
     * @note This function MUST be thread safe because it will be
     * invoked from whichever thread triggered the event.
     */
    virtual void on_service_event(const service_event::Event &) = 0;

  private:
    std::unique_ptr<boost::asio::io_service::work> work_;
    bs2::connection service_event_listener_;

    /**
     * Install handlers that periodically poll for activity on the
     * ZMQ reactor from BaseModule.
     *
     * While this impedes reactivity, it is the less complicated
     * solution for now.
     */
    void install_async_handlers();

    struct StopWatcher : public std::enable_shared_from_this<StopWatcher>
    {
        StopWatcher(AsioModule &self)
            : self_(self)
            , timer_(self.io_service_){};

        void schedule_wait();

      private:
        AsioModule &self_;
        boost::asio::steady_timer timer_;
        void wait(const boost::system::error_code &ec);
    };

    /**
     * Poll the zmq reactor from BaseModule.
     */
    struct AsyncReactorPoller
        : public std::enable_shared_from_this<AsyncReactorPoller>
    {
        AsyncReactorPoller(AsioModule &self)
            : self_(self)
            , timer_(self.io_service_){};

        void schedule_wait();

      private:
        AsioModule &self_;
        boost::asio::steady_timer timer_;
        void wait_handler(const boost::system::error_code &ec);
    };
};
}
}
