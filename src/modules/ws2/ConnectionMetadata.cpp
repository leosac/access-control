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

namespace Leosac
{
namespace Module
{
namespace WS2
{
Leosac::Module::WS2::ConnectionMetadata::ConnectionMetadata()
    : uuid_(gen_uuid())
{
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
    ClientMessage cp = messages_.front();
    messages_.pop();
    return cp;
}

bool ConnectionMetadata::busy_with_queued_msg() const
{
    return busy_handling_queued_message_;
}
}
}
}
