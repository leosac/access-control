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

#include "UpdateDescriptorSerializer.hpp"
#include "core/update/UpdateService.hpp"
#include "tools/JSONUtils.hpp"

namespace Leosac
{
namespace update
{
json UpdateDescriptorJSONSerializer::serialize(const UpdateDescriptor &in)
{
    json serialized;

    serialized["uuid"]          = in.uuid;
    serialized["severity"]      = static_cast<int>(in.severity);
    serialized["source_module"] = in.source_module;
    serialized["update_desc"]   = in.update_desc;

    return serialized;
}
}
}
