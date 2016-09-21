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
#include "core/auth/User.hpp"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Audit;

AuditEntry::AuditEntry()
    : duration_(0)
    , finalized_(false)
    , version_(0)
{
    timestamp_ = boost::posix_time::second_clock::local_time();
}

AuditEntryId AuditEntry::id() const
{
    return id_;
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

void AuditEntry::finalize()
{
    ASSERT_LOG(odb::transaction::has_current(),
               "Not currently in a database transaction.");
    if (finalized_)
    {
        NOTICE(
            "Trying to finalizing already finalized entry. Doing nothing instead.");
        return;
    }
    finalized_ = true;
    ASSERT_LOG(database_, "Null database pointer for AuditEntry.");
    duration_ = etc_.elapsed();
    database_->update(*this);
}

bool AuditEntry::finalized() const
{
    return finalized_;
}

void AuditEntry::event_mask(const EventMask &mask)
{
    ASSERT_LOG(!finalized_, "Audit entry is already finalized.");
    event_mask_ = mask;
}

const EventMask &AuditEntry::event_mask() const
{
    return event_mask_;
}

void AuditEntry::author(Auth::UserPtr user)
{
    if (user)
        ASSERT_LOG(user->id(), "Author is not already persisted.");
    author_ = user;
}

void AuditEntry::set_parent(IAuditEntryPtr parent)
{
    ASSERT_LOG(id_, "Current audit entry must be already persisted.");
    ASSERT_LOG(parent, "Parent must not be null");
    ASSERT_LOG(parent->id(), "Parent must be already persisted.");
    ASSERT_LOG(!parent_.lock(), "Entry must have no parent.");
    auto parent_odb = std::dynamic_pointer_cast<AuditEntry>(parent);
    ASSERT_LOG(parent_odb, "Parent is not of type AuditEntry");

    parent_odb->children_.push_back(shared_from_this());
    parent_ = parent_odb;

    if (!author_)
        author_ = parent_odb->author_;
}

void AuditEntry::remove_parent()
{
    auto parent = parent_.lock();
    if (!parent)
        return;

    auto &children = parent->children_;
    children.erase(std::remove(children.begin(), children.end(), shared_from_this()),
                   children.end());
    parent_.reset();
}

size_t AuditEntry::children_count() const
{
    return children_.size();
}

size_t AuditEntry::version() const
{
    return version_;
}

void AuditEntry::reload()
{
    ASSERT_LOG(odb::transaction::has_current(), "Not currently in transaction.");
    database_->reload(this);
}

IAuditEntryPtr AuditEntry::parent() const
{
    return parent_.lock();
}
