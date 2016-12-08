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

#pragma once

#include "core/audit/AuditFwd.hpp"
#include "core/update/UpdateFwd.hpp"
#include <chrono>

namespace Leosac
{
namespace update
{
class IUpdate
{
  public:
    using TimePoint = std::chrono::system_clock::time_point;

    virtual ~IUpdate() = default;

    virtual UpdateId id() const = 0;

    virtual const TimePoint &generated_at() const = 0;

    virtual Status status() const  = 0;
    virtual void status(Status st) = 0;

    virtual const TimePoint &status_updated_at() const = 0;

    virtual const std::string &source_module() const = 0;

    virtual void source_module(const std::string &s) = 0;
    virtual const std::string &description() const   = 0;

    virtual void description(const std::string &d) = 0;

    /**
     * Set the checkpoint for the update object.
     *
     * The checkpoint represents the timepoint at which this update
     * apply.
     */
    virtual void set_checkpoint(Audit::AuditEntryPtr) = 0;

    virtual Audit::AuditEntryId get_checkpoint() const = 0;
};
}
}
