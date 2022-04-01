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

#include "Worker.hpp"
#include "core/auth/Auth.hpp"
#include "exception/ExceptionsTools.hpp"
#include "tools/enforce.hpp"
#include <boost/regex.hpp>
#include <logicalaccess/bufferhelper.hpp>
#include <logicalaccess/dynlibrary/librarymanager.hpp>
#include <logicalaccess/myexception.hpp>
#include <logicalaccess/readerproviders/readerconfiguration.hpp>
#include <logicalaccess/services/accesscontrol/formats/format.hpp>

namespace Leosac
{
namespace Module
{

namespace LLA
{

Worker::Worker(const WorkerConfiguration &cfg, zmqpp::context &ctx)
    : cfg_(cfg)
    , zmq_ctx_(ctx)

{
  thread_ = std::make_unique<std::thread>([this]() { this->run(); });
}

Worker::~Worker()
{
  if (run_)
    stop();
  thread_->join();
}

void Worker::stop()
{
  run_ = false;
}

bool Worker::poll_card(std::string &card_id_out)
{
  bool found{false};

  // We use ENFORCE to manually throw LibLogicalAccessException.
  // This allows us to properly filter error in `run()` and take
  // an appropriate decision.

  auto readerConfig = cfg_.readerformat.getReaderConfiguration();

  if (!readerConfig->getReaderUnit()->connectToReader())
  {
    THROW_EXCEPTION_WITH_LOG(logicalaccess::LibLogicalAccessException, "Failed to connect to reader");
  }

  if (readerConfig->getReaderUnit()->waitInsertion(cfg_.polling_time))
  {
    WARN("Card detected. On reader: "
         << readerConfig->getReaderUnit()->getConnectedName());
    if (readerConfig->getReaderUnit()->connect())
    {
      auto format = cfg_.readerformat.readFormat();
      if (format)
      {
        card_id_out = logicalaccess::BufferHelper::getHex(format->getLinearData());
      }
      else
      {
        auto chip =
            readerConfig->getReaderUnit()->getSingleChip();

        card_id_out = logicalaccess::BufferHelper::getHex(chip->getChipIdentifier());
      }
      found = true;

      if (!readerConfig->getReaderUnit()->waitRemoval(cfg_.polling_time))
      {
        WARN("Card removal forced.");
      }
      readerConfig->getReaderUnit()->disconnect();
      readerConfig->getReaderUnit()->disconnectFromReader();
    }
  }
  return found;
}

void Worker::run()
{
  bus_push_ = std::make_unique<zmqpp::socket>(zmq_ctx_, zmqpp::socket_type::push);
  bus_push_->connect("inproc://zmq-bus-pull");

  while (run_)
  {
    try
    {
      std::string card_id;
      if (poll_card(card_id))
      {
        std::string csn_formated;
        int count = 0;
        for (auto &c : card_id)
        {
          if (count && count % 2 == 0)
              csn_formated.push_back(':');
          csn_formated.push_back(c);
          count++;
        }
        signal(csn_formated);
      }
    }
    catch (const logicalaccess::CardException &e)
    {
      ERROR("LogicalAccess encountered a card error: " << e.what());
      print_exception(e);
      // Let us retry immediately.
    }
    catch (const logicalaccess::LibLogicalAccessException &e)
    {
      ERROR("LogicalAccess encountered an error: " << e.what());
      print_exception(e);
      // We sleep for a few seconds, then keep trying.
      std::this_thread::sleep_for(std::chrono::seconds(4));
    }
    catch (const std::exception &e)
    {
      ERROR("General error in LLA worker: " << e.what());
      print_exception(e);
      // We die.
      throw;
    }
  }
}

void Worker::signal(const std::string &csn)
{
  zmqpp::message msg;
  std::string source = "S_" + cfg_.source_name;
  msg << source << Leosac::Auth::SourceType::SIMPLE_CSN << csn;

  ASSERT_LOG(bus_push_, "Socket shall not be null.");
  bus_push_->send(msg);
}
}
}
}
