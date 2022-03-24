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

#include "AuditSerializer.hpp"
#include "core/audit/IAuditEntry.hpp"
#include <boost/date_time/posix_time/conversion.hpp>

namespace Leosac
{
namespace Audit
{
namespace Serializer
{
json AuditJSON::serialize(const Audit::IAuditEntry &in, const SecurityContext &)
{
    auto timestamp = boost::posix_time::to_time_t(in.timestamp());

    json serialized = {
        {"id", in.id()},
        {"type", "audit-entry"},
        {"attributes",
         {
             {"event-mask", static_cast<unsigned long>(in.event_mask())},
             {"timestamp", timestamp},
             {"description", in.generate_description()},
             {"finalized", in.finalized()},
         }}};

    if (in.author_id())
    {
        serialized["relationships"]["author"] = {
            {"data", {{"id", in.author_id()}, {"type", "user"}}}};
    }
    return serialized;
}
}
}
}
