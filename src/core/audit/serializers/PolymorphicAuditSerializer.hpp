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

#pragma once

#include "core/audit/IAuditEntry.hpp"
#include "tools/Serializer.hpp"
#include "tools/Visitor.hpp"
#include <json.hpp>
#include <string>

namespace Leosac
{
using json = nlohmann::json;

/**
 * A serializer that handle any type of Audit event and will
 * try to perform deep serialization.
 */
struct PolymorphicAuditJSONSerializer
    : public Serializer<json, Audit::IAuditEntry, PolymorphicAuditJSONSerializer>
{
    static json serialize(const Audit::IAuditEntry &in, const SecurityContext &sc);

    /**
     * Returns the "type-name" of the audit entry.
     *
     * Possible return value could be "audit-user-event".
     */
    static std::string type_name(const Audit::IAuditEntry &in);

  private:
    /**
     * Non static helper that can visit audit object.
     */
    struct HelperSerialize : public Tools::Visitor<Audit::IUserEvent>,
                             public Tools::Visitor<Audit::IWSAPICall>,
                             public Tools::Visitor<Audit::IScheduleEvent>,
                             public Tools::Visitor<Audit::IGroupEvent>,
                             public Tools::Visitor<Audit::ICredentialEvent>,
                             public Tools::Visitor<Audit::IDoorEvent>
    {
        HelperSerialize(const SecurityContext &sc);

        void visit(const Audit::IUserEvent &t) override;
        void visit(const Audit::IWSAPICall &t) override;
        void visit(const Audit::IScheduleEvent &t) override;
        void visit(const Audit::IGroupEvent &t) override;
        void visit(const Audit::ICredentialEvent &t) override;

        void visit(const Audit::IDoorEvent &t) override;

        /**
     * Store the result here because we can't return from
     * the visit() method.
     */
        json result_;

        /**
         * Reference to the security context.
         */
        const SecurityContext &security_context_;
    };
};
}
