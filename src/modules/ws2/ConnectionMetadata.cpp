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

#include "modules/ws2/ConnectionMetadata.hpp"
#include "core/SecurityContext.hpp"
#include "tools/GenGuid.h"
#include "tools/log.hpp"
#include <core/UserSecurityContext.hpp>

namespace Leosac
{
namespace Module
{
namespace WS2
{

ConnectionMetadata::ConnectionMetadata(
    std::shared_ptr<boost::asio::strand> connection_strand,
    websocketpp::connection_hdl connection_handle)
    : uuid_(gen_uuid())
    , connection_strand_(connection_strand)
    , connection_hdl_(connection_handle)
{
    ASSERT_LOG(connection_strand_, "Connection strand is null, but shouldn't be.");
}


bool ConnectionMetadata::has_pending_messages() const
{
    return messages_.size() > 0;
}

void ConnectionMetadata::enqueue(const ClientMessage &msg)
{
    messages_.push(msg);
}

ClientMessage ConnectionMetadata::dequeue()
{
    ASSERT_LOG(has_pending_messages(), "No message to dequeue.");
    ClientMessage cp = messages_.front();
    messages_.pop();
    return cp;
}

bool ConnectionMetadata::is_busy_for_serial() const
{
    return !ready_for_serial_;
}

void ConnectionMetadata::mark_ready_for_serial()
{
    ready_for_serial_ = true;
}

void ConnectionMetadata::mark_busy_for_serial()
{
    ready_for_serial_ = false;
}

void ConnectionMetadata::incr_msg_count()
{
    ++msg_count_;
}

size_t ConnectionMetadata::msg_count() const
{
    return msg_count_;
}

SecurityContextCPtr ConnectionMetadata::security_context() const
{
    if (security_)
        return security_;
    return std::make_shared<NullSecurityContext>();
}

const std::shared_ptr<boost::asio::strand> &ConnectionMetadata::strand() const
{
    ASSERT_LOG(connection_strand_, "Connection strand is null, but shouldn't be.");
    return connection_strand_;
}

const websocketpp::connection_hdl &ConnectionMetadata::handle() const
{
    return connection_hdl_;
}
}
}
}
