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

#include <memory>
#include <odb/lazy-ptr.hxx>

namespace Leosac
{
namespace update
{

enum class Status
{
    PENDING      = 0,
    ACKNOWLEDGED = 1,
    CANCELLED    = 2,

    // unused
    TRANSIENT = 3
};


class IUpdate;
using IUpdatePtr = std::shared_ptr<IUpdate>;

class Update;
using UpdatePtr   = std::shared_ptr<Update>;
using UpdateLWPtr = odb::lazy_weak_ptr<Update>;

class UpdateBackend;
using UpdateBackendPtr = std::shared_ptr<UpdateBackend>;

struct UpdateDescriptor;
using UpdateDescriptorPtr = std::shared_ptr<UpdateDescriptor>;

class UpdateService;

using UpdateId = unsigned long;
}
}
