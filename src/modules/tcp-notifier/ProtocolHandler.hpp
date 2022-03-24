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

#include "LeosacFwd.hpp"
#include "core/credentials/CredentialFwd.hpp"
#include <stdexcept>

namespace Leosac
{
namespace Module
{
namespace TCPNotifier
{
enum Protocol
{
    SIMPLE_CARD_NUMBER,
};

class ProtocolHandler;
using ProtocolHandlerUPtr = std::unique_ptr<ProtocolHandler>;


/**
 * An specialized exception that ProtocolHandler can
 * throw when converting the credential to a
 * message failed.
 */
class ProtocolException : public ::std::runtime_error
{
  public:
    ProtocolException(const std::string &msg)
        : runtime_error(msg)
    {
    }
};

/**
 * Implements a given (TCP) protocol for notifying
 * clients.
 *
 * The ProtocolHandler is supposed
 * to format on-the-wire data to notify a client of
 * a credential read.
 *
 * One instance of protocol handler is created by client.
 */
class ProtocolHandler
{
  protected:
    /**
     * ProtocolHandler shall not be created directly.
     * Call `create()` instead.
     */
    ProtocolHandler(){};

  public:
    /**
     * Returns a binary buffer containing the data
     * that shall be sent over the network to notify
     * the client.
     */
    virtual ByteVector build_cred_msg(const Cred::RFIDCard &card) = 0;

    /**
     * Create an instance of a protocol handler depending
     * on the requested protocol id.
     */
    static ProtocolHandlerUPtr create(int protocol_id);
};
}
}
}
