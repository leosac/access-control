/*
    Copyright (C) 2014-2022 Leosac

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
#include "modules/lla/LLAFwd.hpp"
#include <logicalaccess/services/accesscontrol/readerformatcomposite.hpp>

namespace Leosac
{
namespace Module
{
namespace LLA
{

/**
 * Configuration for a single LLA worker.
 */
struct WorkerConfiguration
{
    std::string source_name;
    unsigned int polling_time;
    logicalaccess::ReaderFormatComposite readerformat;
};

/**
 * A worker, running in its own thread and
 * performing liblogicalaccess operation.
 *
 * When a card has been successfully polled, the worker
 * will push the information on the application message bus, respecting
 * the protocol for authentication source module.
 *
 * The worker poll in an infinite loop, until stop() is called.
 */
class Worker
{
  public:
    Worker(const WorkerConfiguration &cfg, zmqpp::context &ctx);
    ~Worker();

    /**
     * A thread-safe function to order the worker to stop its
     * polling loop. The worker's thread will not stop until
     * the current waitInsertion operation terminates.
     */
    void stop();

  private:
    /**
     * A card has been read. Signal the application.
     * @param csn The serial number of the card.
     */
    void signal(const std::string &csn);

    void run();

    bool poll_card(std::string &card_id_out);

    std::atomic_bool run_{true};

    WorkerConfiguration cfg_;

    zmqpp::context &zmq_ctx_;

    /**
     * Socket to write the bus.
     *
     * The socket lives in the worker's thread.
     */
    std::unique_ptr<zmqpp::socket> bus_push_;

    std::unique_ptr<std::thread> thread_;
};
}
}
}
