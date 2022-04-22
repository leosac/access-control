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

#include "AuthEvent.hpp"
#include "LeosacFwd.hpp"
#include "core/audit/AuthEvent_odb.h"
#include "core/auth/AuthSourceBuilder.hpp"
#include "core/SecurityContext.hpp"
#include "core/credentials/serializers/PolymorphicCredentialSerializer.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/db/OptionalTransaction.hpp"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Audit;

AuthEvent::AuthEvent()
    : cred_id_(0)
    , access_point_id_(0)
{
}

std::shared_ptr<AuthEvent> AuthEvent::create(const DBPtr &database,
                                             Cred::ICredentialPtr credential,
                                             Auth::IAccessPointPtr access_point,
                                             AuditEntryPtr parent)
{
    ASSERT_LOG(database, "Database cannot be null.");
    ASSERT_LOG(credential, "Credential must be non null.");
    ASSERT_LOG(access_point, "AccessPoint must be non null.");
    ASSERT_LOG(access_point->id(), "AccessPoint must be already persisted.");
    ASSERT_LOG(parent, "Parent must be non null.");
    ASSERT_LOG(parent->id(), "Parent must be already persisted.");

    db::OptionalTransaction t(database->begin());

    Audit::AuthEventPtr audit =
        std::shared_ptr<Audit::AuthEvent>(new Audit::AuthEvent());
    audit->database_ = database;
    audit->access_point_id(access_point->id());
    audit->credential(credential);
    database->persist(audit);

    audit->set_parent(parent);
    database->update(audit);

    t.commit();
    return audit;
}

void AuthEvent::credential(Cred::ICredentialPtr cred)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");

    cred_raw_ = PolymorphicCredentialJSONStringSerializer::serialize(
        *cred, SystemSecurityContext::instance());;
    if (cred->id())
    {
      auto cred_odb = std::dynamic_pointer_cast<Cred::Credential>(cred);
      ASSERT_LOG(cred_odb, "ICredential is not of type Credential.");

      cred_    = cred_odb;
      cred_id_ = cred->id();
    }
}

std::string AuthEvent::generate_description() const
{
    std::stringstream ss;

    if (event_mask_ & EventType::AUTH_GRANTED)
        ss << "Access Granted to Credential " << generate_credential_description() << ".";
    else if (event_mask_ & EventType::AUTH_DENIED)
        ss << "Access Denied to Credential " << generate_credential_description() << ".";

    return ss.str();
}

std::string AuthEvent::generate_credential_description() const
{
    Leosac::json desc;

    desc["id"] = credential_id();
    auto c     = cred_.load();
    if (c)
    {
      desc["alias"] = c->alias();
    }

    return desc.dump();
}

std::shared_ptr<AuthEvent> AuthEvent::create_empty()
{
    return std::shared_ptr<AuthEvent>(new AuthEvent());
}

std::string AuthEvent::credential_raw() const
{
    return cred_raw_;
}

Cred::CredentialId AuthEvent::credential_id() const
{
    return cred_id_;
}

Auth::AccessPointId AuthEvent::access_point_id() const
{
    return access_point_id_;
}

void AuthEvent::access_point_id(Auth::AccessPointId id)
{
    access_point_id_ = id;
}

Auth::AccessStatus AuthEvent::access_status() const
{
    return access_status_;
}

void AuthEvent::access_status(Auth::AccessStatus access_status)
{
    access_status_ = access_status;
}
