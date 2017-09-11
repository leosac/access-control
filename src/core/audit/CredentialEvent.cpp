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

#include "core/audit/CredentialEvent.hpp"
#include "core/audit/CredentialEvent_odb.h"
#include "core/credentials/Credential.hpp"
#include "core/credentials/Credential_odb.h"
#include "core/credentials/serializers/PolymorphicCredentialSerializer.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/db/OptionalTransaction.hpp"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Audit;

std::shared_ptr<CredentialEvent>
CredentialEvent::create(const DBPtr &database, Cred::ICredentialPtr target_cred,
                        AuditEntryPtr parent)
{
    ASSERT_LOG(database, "Database cannot be null.");
    ASSERT_LOG(target_cred, "Credential shall not be null.");
    ASSERT_LOG(target_cred->id(), "Credential must be already persisted.");

    db::OptionalTransaction t(database->begin());

    Audit::CredentialEventPtr audit =
        std::shared_ptr<Audit::CredentialEvent>(new Audit::CredentialEvent());
    audit->database_ = database;
    audit->target(target_cred);
    database->persist(audit);

    audit->set_parent(parent);
    database->update(audit);

    t.commit();
    return audit;
}

void CredentialEvent::target(Cred::ICredentialPtr cred)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    if (cred)
        ASSERT_LOG(cred->id(), "Credential has no id.");

    auto cred_odb = std::dynamic_pointer_cast<Cred::Credential>(cred);
    ASSERT_LOG(cred_odb, "ICredential is not of type Credential.");

    target_         = cred_odb;
    target_cred_id_ = cred->id();
}

void CredentialEvent::before(const std::string &repr)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    before_ = repr;
}

void CredentialEvent::after(const std::string &repr)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    after_ = repr;
}

Cred::CredentialId CredentialEvent::target_id() const
{
    if (target_.lock())
    {
        return target_.object_id();
    }
    return 0;
}

const std::string &CredentialEvent::before() const
{
    return before_;
}

const std::string &CredentialEvent::after() const
{
    return after_;
}

std::string CredentialEvent::generate_description() const
{
    using namespace FlagSetOperator;
    std::stringstream ss;

    auto target = target_.load();
    auto author = author_.load();

    if (event_mask_ & EventType::CREDENTIAL_CREATED)
        ss << "Credential " << generate_target_description() << " has been created.";
    else if (event_mask_ & EventType::CREDENTIAL_UPDATED)
        ss << "Credential " << generate_target_description() << " has been edited.";
    else if (event_mask_ & EventType::CREDENTIAL_DELETED)
        ss << "Credential " << generate_target_description() << " has been deleted.";

    return ss.str();
}

std::string CredentialEvent::generate_target_description() const
{
    Leosac::json desc;

    desc["id"] = target_id();
    auto t     = target_.load();
    if (t)
    {
        desc["alias"] = t->alias();
        desc["type"]  = PolymorphicCredentialJSONSerializer::type_name(*t);
    }
    return desc.dump();
}

std::shared_ptr<CredentialEvent> CredentialEvent::create_empty()
{
    return std::shared_ptr<CredentialEvent>(new CredentialEvent());
}
