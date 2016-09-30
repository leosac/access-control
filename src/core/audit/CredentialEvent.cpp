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

#include "core/audit/CredentialEvent.hpp"
#include "CredentialEvent_odb.h"
#include "Credential_odb.h"
#include "core/credentials/Credential.hpp"
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
        ASSERT_LOG(cred->id(), "Group has no id.");

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
