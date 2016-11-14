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

#include "JSONService.hpp"
#include "core/audit/IAuditEntry.hpp"

namespace Leosac
{
namespace Audit
{
namespace Serializer
{
JSONService::~JSONService()
{
    // Sanity check: All serializers have been unregistered.
    ASSERT_LOG(
        serializers_.size() == 0,
        "Audit::Serializer::JSONService still has some unregistered serializer.");
}

json JSONService::serialize(const Audit::IAuditEntry &audit,
                            const SecurityContext &sc)
{
    auto type_index = boost::typeindex::type_id_runtime(audit);
    auto itr        = serializers_.find(type_index);
    if (itr != serializers_.end())
    {
        // Invoke the adapter we stored in the map.
        // The wrapper will invoke the user-defined callable.
        return itr->second(audit, sc);
    }
    ASSERT_LOG(false, "Cannot find an appropriate serializer for " +
                          type_index.pretty_name());
    return {};
}
}
}
}
