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

#include "UpdateSerializer.hpp"
#include "core/update/IUpdate.hpp"
#include "tools/Conversion.hpp"
#include "tools/JSONUtils.hpp"

namespace Leosac
{
namespace update
{
json UpdateJSONSerializer::serialize(const update::IUpdate &in,
                                     const SecurityContext &)
{
    json serialized;
    serialized["type"] = "update";
    serialized["id"]   = in.id();

    serialized["attributes"]["status"]     = static_cast<int>(in.status());
    serialized["attributes"]["checkpoint"] = in.get_checkpoint();
    serialized["attributes"]["generated-at"] =
        Conversion<std::string>(in.generated_at());
    serialized["attributes"]["status-updated-at"] =
        Conversion<std::string>(in.status_updated_at());

    serialized["attributes"]["source-module"] = in.source_module();
    serialized["attributes"]["description"]   = in.description();

    return serialized;
}
}
}
