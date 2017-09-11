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

#include "Update.hpp"

namespace Leosac
{
namespace update
{

Update::Update()
    : id_(0)
    , generated_at_(std::chrono::system_clock::now())
    , status_(Status::TRANSIENT)
    , odb_version_(0)
{
}

UpdateId Update::id() const
{
    return id_;
}

const IUpdate::TimePoint &Update::generated_at() const
{
    return generated_at_;
}


Status Update::status() const
{
    return status_;
}

void Update::status(Status st)
{
    status_updated_at_ = std::chrono::system_clock::now();
    status_            = st;
}

const IUpdate::TimePoint &Update::status_updated_at() const
{
    return status_updated_at_;
}

void Update::set_checkpoint(Audit::AuditEntryPtr audit)
{
    checkpoint_.last(audit);
}

Audit::AuditEntryId Update::get_checkpoint() const
{
    return checkpoint_.last_id();
}

const std::string &Update::source_module() const
{
    return source_module_;
}

void Update::source_module(const std::string &s)
{
    source_module_ = s;
}

const std::string &Update::description() const
{
    return description_;
}
void Update::description(const std::string &d)
{
    description_ = d;
}
}
}
