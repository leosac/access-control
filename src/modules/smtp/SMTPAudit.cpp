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

#include "SMTPAudit.hpp"
#include "SMTPAudit_odb.h"
#include "core/audit/AuditEntry.hpp"
#include "tools/AssertCast.hpp"
#include "tools/db/OptionalTransaction.hpp"
#include "tools/log.hpp"

namespace Leosac
{
namespace Module
{

namespace SMTP
{

SMTPAudit::SMTPAudit(const SMTPAudit::ConstructorAccess &)
{
}

std::string SMTPAudit::generate_description() const
{
    return "An SMTP event happened.";
}

SMTPAuditPtr SMTPAudit::create(const DBPtr &database, Audit::IAuditEntryPtr parent)
{
    ASSERT_LOG(database, "Database cannot be null.");

    db::OptionalTransaction t(database->begin());
    auto audit       = std::make_shared<SMTPAudit>(SMTPAudit::ConstructorAccess{});
    audit->database_ = database;
    database->persist(*audit);

    if (parent)
    {
        ASSERT_LOG(parent->id(), "Parent must be already persisted.");
        Audit::AuditEntryPtr parent_odb = assert_cast<Audit::AuditEntryPtr>(parent);
        audit->set_parent(parent_odb);
        database->update(*audit);
    }
    t.commit();
    return audit;
}
};
}
}
