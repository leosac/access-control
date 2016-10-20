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

#include "core/audit/serializers/PolymorphicAuditSerializer.hpp"
#include "CredentialEventSerializer.hpp"
#include "DoorEventSerializer.hpp"
#include "GroupEventSerializer.hpp"
#include "ScheduleEventSerializer.hpp"
#include "WSAPICallSerializer.hpp"
#include "core/SecurityContext.hpp"
#include "core/audit/serializers/UserEventSerializer.hpp"
#include "core/audit/serializers/UserGroupMembershipEventSerializer.hpp"
#include "tools/JSONUtils.hpp"

using namespace Leosac;

json PolymorphicAuditJSONSerializer::serialize(const Audit::IAuditEntry &in,
                                               const SecurityContext &sc)
{
    HelperSerialize h(sc);
    in.accept(h);
    return h.result_;
}

std::string PolymorphicAuditJSONSerializer::type_name(const Audit::IAuditEntry &in)
{
    HelperSerialize h(SystemSecurityContext::instance());
    in.accept(h);
    ASSERT_LOG(h.result_.find("type") != h.result_.end(),
               "The serializer didn't set a type.");
    ASSERT_LOG(h.result_.at("type").is_string(), "Type is not a string.");
    return h.result_.at("type");
}

PolymorphicAuditJSONSerializer::HelperSerialize::HelperSerialize(
    const SecurityContext &sc)
    : security_context_(sc)
{
}

void PolymorphicAuditJSONSerializer::HelperSerialize::visit(
    const Audit::IUserEvent &t)
{
    result_ = UserEventJSONSerializer::serialize(t, security_context_);
}

void PolymorphicAuditJSONSerializer::HelperSerialize::visit(
    const Audit::IWSAPICall &t)
{
    result_ = WSAPICallJSONSerializer::serialize(t, security_context_);
}

void PolymorphicAuditJSONSerializer::HelperSerialize::visit(
    const Audit::IScheduleEvent &t)
{
    result_ = ScheduleEventJSONSerializer::serialize(t, security_context_);
}

void PolymorphicAuditJSONSerializer::HelperSerialize::visit(
    const Audit::IGroupEvent &t)
{
    result_ = GroupEventJSONSerializer::serialize(t, security_context_);
}

void PolymorphicAuditJSONSerializer::HelperSerialize::visit(
    const Audit::ICredentialEvent &t)
{
    result_ = CredentialEventJSONSerializer::serialize(t, security_context_);
}

void PolymorphicAuditJSONSerializer::HelperSerialize::visit(
    const Audit::IDoorEvent &t)
{
    result_ = DoorEventJSONSerializer::serialize(t, security_context_);
}

void PolymorphicAuditJSONSerializer::HelperSerialize::visit(
    const Audit::IUserGroupMembershipEvent &t)
{
    result_ =
        UserGroupMembershipEventJSONSerializer::serialize(t, security_context_);
}
