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

#include "core/audit/serializers/PolymorphicAuditSerializer.hpp"
#include "CredentialEventSerializer.hpp"
#include "DoorEventSerializer.hpp"
#include "AuthEventSerializer.hpp"
#include "GroupEventSerializer.hpp"
#include "JSONService.hpp"
#include "ScheduleEventSerializer.hpp"
#include "UpdateEventSerializer.hpp"
#include "WSAPICallSerializer.hpp"
#include "ZoneEventSerializer.hpp"
#include "core/GetServiceRegistry.hpp"
#include "core/SecurityContext.hpp"
#include "core/audit/serializers/UserEventSerializer.hpp"
#include "core/audit/serializers/UserGroupMembershipEventSerializer.hpp"
#include "tools/AssertCast.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/registry/GlobalRegistry.hpp"

namespace Leosac
{
namespace Audit
{
namespace Serializer
{

json PolymorphicAuditJSON::serialize(const Audit::IAuditEntry &in,
                                     const SecurityContext &sc)
{
    HelperSerialize h(sc);
    in.accept(h);
    return h.result_;
}

std::string PolymorphicAuditJSON::type_name(const Audit::IAuditEntry &in)
{
    HelperSerialize h(SystemSecurityContext::instance());
    in.accept(h);
    ASSERT_LOG(h.result_.find("type") != h.result_.end(),
               "The serializer didn't set a type.");
    ASSERT_LOG(h.result_.at("type").is_string(), "Type is not a string.");
    return h.result_.at("type");
}

PolymorphicAuditJSON::HelperSerialize::HelperSerialize(const SecurityContext &sc)
    : security_context_(sc)
{
}

void PolymorphicAuditJSON::HelperSerialize::visit(const Audit::IUserEvent &t)
{
    result_ = UserEventJSON::serialize(t, security_context_);
}

void PolymorphicAuditJSON::HelperSerialize::visit(const Audit::IWSAPICall &t)
{
    result_ = WSAPICallJSON::serialize(t, security_context_);
}

void PolymorphicAuditJSON::HelperSerialize::visit(const Audit::IScheduleEvent &t)
{
    result_ = ScheduleEventJSON::serialize(t, security_context_);
}

void PolymorphicAuditJSON::HelperSerialize::visit(const Audit::IGroupEvent &t)
{
    result_ = GroupEventJSON::serialize(t, security_context_);
}

void PolymorphicAuditJSON::HelperSerialize::visit(const Audit::ICredentialEvent &t)
{
    result_ = CredentialEventJSON::serialize(t, security_context_);
}

void PolymorphicAuditJSON::HelperSerialize::visit(const Audit::IDoorEvent &t)
{
    result_ = DoorEventJSON::serialize(t, security_context_);
}

void PolymorphicAuditJSON::HelperSerialize::visit(const Audit::IAuthEvent &t)
{
    result_ = AuthEventJSON::serialize(t, security_context_);
}

void PolymorphicAuditJSON::HelperSerialize::visit(
    const Audit::IUserGroupMembershipEvent &t)
{
    result_ = UserGroupMembershipEventJSON::serialize(t, security_context_);
}

void PolymorphicAuditJSON::HelperSerialize::visit(const Audit::IUpdateEvent &t)
{
    result_ = UpdateEventJSON::serialize(t, security_context_);
}

void PolymorphicAuditJSON::HelperSerialize::visit(const Audit::IZoneEvent &t)
{
    result_ = ZoneEventJSON::serialize(t, security_context_);
}

void PolymorphicAuditJSON::HelperSerialize::cannot_visit(
    const Tools::IVisitable &visitable)
{
    // If we cannot visit a visitable it may mean 2 things:
    //   + It is an Audit object defined in a module. In that case we'll rely
    //       on dynamically registered serializer.
    //   + Its an other object, and in that case, we shouldn't be here (we will
    //       assert)
    const auto &audit_entry = assert_cast<const Audit::IAuditEntry &>(visitable);
    auto service =
        get_service_registry().get_service<Audit::Serializer::JSONService>();
    ASSERT_LOG(service,
               "Cannot retrieve Audit::Serializer::JSONService. Since this a "
               "core service, something must be very wrong.");

    result_ = service->serialize(audit_entry, security_context_);
}
}
}
}
