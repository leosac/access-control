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

#pragma once

#define ODB_NO_BASE_VERSION
#include "SMTPFwd.hpp"
#include "core/audit/AuditEntry.hpp"

#pragma db model version(1, 1, open)

namespace Leosac
{
namespace Module
{

namespace SMTP
{
/**
 * Keeps track of SMTP event.
 */
#pragma db object polymorphic callback(odb_callback) table("SMTP_Audit")
class SMTPAudit : public Audit::AuditEntry
{
  private:
    MAKE_VISITABLE();

    /**
     * A tag struct to be able to call std::make_shared() despite
     * a private ctor.
     */
    struct ConstructorAccess
    {
    };

    SMTPAudit() = default;

  public:
    explicit SMTPAudit(const ConstructorAccess &);

    virtual std::string generate_description() const override;

    /**
     * Factory function, similar to those found in Audit::Factory
     */
    static SMTPAuditPtr create(const DBPtr &database, Audit::IAuditEntryPtr parent);

  private:
    friend class odb::access;
};
}
}
}
