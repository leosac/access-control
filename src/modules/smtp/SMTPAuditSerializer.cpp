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

#include "SMTPAuditSerializer.hpp"
#include "core/audit/serializers/AuditSerializer.hpp"

namespace Leosac
{
namespace Module
{
namespace SMTP
{


json SMTPAuditSerializer::serialize(const SMTPAudit &in, const SecurityContext &sc)
{
    auto serialized = Audit::Serializer::AuditJSON::serialize(in, sc);
    // Now we override the type.
    ASSERT_LOG(serialized.at("type").is_string(),
               "Base audit serialization did something unexpected.");
    serialized["type"] = "smtp-event";

    return serialized;
}

SMTPAuditSerializer::Helper::Helper(const SecurityContext &sc)
    : has_visited_(false)
    , sc_(sc)
{
}

void SMTPAuditSerializer::Helper::cannot_visit(const Tools::IVisitable &visitable)
{
    ASSERT_LOG(!has_visited_, "Already visited something. This object has been "
                              "reused. Something is wrong.");
    // It's important that we don't care about not being able to visit.
    // Since we are runtime-registered, its likely that we'll be attempting
    // to visit audit object that we don't care about.
}

void SMTPAuditSerializer::Helper::visit(const SMTPAudit &t)
{
    ASSERT_LOG(!has_visited_, "Already visited something. This object has been "
                              "reused. Something is wrong.");
    has_visited_ = true;
    result_      = SMTPAuditSerializer::serialize(t, sc_);
}
}
}
}