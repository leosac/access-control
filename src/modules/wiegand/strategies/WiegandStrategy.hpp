/*
    Copyright (C) 2014-2016 Islog

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

#include "core/auth/Auth.hpp"
#include "zmqpp/zmqpp.hpp"
#include <boost/any.hpp>
#include <chrono>
#include <string>

namespace Leosac
{
namespace Module
{
namespace Wiegand
{
class WiegandReaderImpl;
namespace Strategy
{
class WiegandStrategy;
using WiegandStrategyUPtr = std::unique_ptr<WiegandStrategy>;

/**
* The multiple modes available to wiegand reader are implemented through the
* strategy pattern.
*
* This class defines the interface.
*
* @note: Currently the strategy is only to handle wiegand timeout (2ms) not the
* reception
* of each GPIO tick.
*/
class WiegandStrategy
{
  public:
    WiegandStrategy(WiegandReaderImpl *reader)
        : reader_(reader)
    {
    }

    virtual ~WiegandStrategy() = default;

    /**
    * This is called when the module detect a timeout. (2 ms of inactivity).
    */
    virtual void timeout() = 0;

    /**
    * Did the strategy gather needed data?
    * If this function returns true, that means that the strategy implementation
    * successfully retrieve data from wiegand bits.
    *
    * It successfully build a card_id, or a PIN code, etc.
    * The reader implementation will call `signal()` if `completed()` returns true.
    *
    */
    virtual bool completed() const = 0;

    /**
    * Tells the strategy implementation to send a message to the application
    * containing the received credentials.
    *
    * It is up to the strategy to format a correct message. This is required because
    * only the strategy knows what kind of credential it can generate.
    *
    * @param sock the socket where we write the message containing credentials.
    */
    virtual void signal(zmqpp::socket &sock) = 0;

    /**
    * Reset the strategy, meaning that the next time timeout() is called
    * the behavior should be the same than the first time.
    *
    * Basically, implementation should wipe its state (parts of PIN code read, card
    * ID, etc).
    */
    virtual void reset() = 0;

    /**
    * Update the pointer that points back to the associated reader.
    */
    virtual void set_reader(WiegandReaderImpl *new_ptr)
    {
        reader_ = new_ptr;
    }

    // we may add the handle_msg here, because some mode may want to
    // have "lower-level" access to gpio event.

    friend class WiegandReaderImpl;

  protected:
    WiegandReaderImpl *reader_;
};
}
}
}
}
