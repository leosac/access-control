/*
    Copyright (C) 2014-2016 Islog

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

#include "AuditEntry.hpp"
#include "AuditEntry_odb.h"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Audit;

AuditEntry::AuditEntry()
    : version_(0)
{
    timestamp_ = boost::posix_time::second_clock::local_time();
}

AuditEntryId AuditEntry::id() const
{
    return id_;
}

void AuditEntry::set_parent(AuditEntryPtr parent)
{
    ASSERT_LOG(parent, "Parent must not be null");
    ASSERT_LOG(parent->id(), "Parent must be already persisted.");
    ASSERT_LOG(!parent_.lock(), "Entry must have no parent.");

    auto self = shared_from_this();
    assert(self);
    parent->children_.push_back(self);
    parent_ = parent;

    if (!author_)
        author_ = parent->author_;
}

void AuditEntry::odb_callback(odb::callback_event e, odb::database &db) const
{
    if (e == odb::callback_event::post_update ||
        e == odb::callback_event::post_persist)
    {
        if (auto parent = parent_.lock())
        {
            ASSERT_LOG(parent->id(), "Parent must be already persisted.");
            db.update(parent);
        }
    }
}
