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

#include "LeosacFwd.hpp"
#include "core/audit/IAuditEntry.hpp"
#include "tools/Visitor.hpp"
#include <json.hpp>
#include <string>

namespace Leosac
{
using json = nlohmann::json;

namespace Audit
{
namespace Serializer
{
/**
 * A serializer that handle any type of Audit event and will
 * try to perform deep serialization.
 *
 * This class, through the use of other classes in the Audit::Serializer
 * namespace, is able to serialize core Audit objects.
 *
 * However, the audit system being extensible, we need to be able to use
 * externally registered serializer. This is why the `serialize()` method
 * takes an additional ServiceRegistry parameter: in the case where the
 * to-be-serialized entry is not part of the core, we'll attempt to serialize
 * it through the Audit::Serializer::JSONService class.
 */
struct PolymorphicAuditJSON
{
    /**
     * Perform deep serialization of the AuditEntry `in`.
     */
    static json serialize(const Audit::IAuditEntry &in, const SecurityContext &sc);

    /**
     * Returns the "type-name" of the audit entry.
     *
     * Possible return value could be "audit-user-event".
     */
    static std::string type_name(const Audit::IAuditEntry &in);

  private:
    /**
     * Non static helper that can visit a core audit object.
     */
    struct HelperSerialize : public Tools::Visitor<Audit::IUserEvent>,
                             public Tools::Visitor<Audit::IWSAPICall>,
                             public Tools::Visitor<Audit::IScheduleEvent>,
                             public Tools::Visitor<Audit::IGroupEvent>,
                             public Tools::Visitor<Audit::ICredentialEvent>,
                             public Tools::Visitor<Audit::IDoorEvent>,
                             public Tools::Visitor<Audit::IUserGroupMembershipEvent>,
                             public Tools::Visitor<Audit::IUpdateEvent>
    {
        HelperSerialize(const SecurityContext &sc);

        virtual void visit(const Audit::IUserEvent &t) override;
        virtual void visit(const Audit::IWSAPICall &t) override;
        virtual void visit(const Audit::IScheduleEvent &t) override;
        virtual void visit(const Audit::IGroupEvent &t) override;
        virtual void visit(const Audit::ICredentialEvent &t) override;
        virtual void visit(const Audit::IDoorEvent &t) override;
        virtual void visit(const Audit::IUserGroupMembershipEvent &t) override;
        virtual void visit(const Audit::IUpdateEvent &t) override;

        /**
         * Called when no "hardcoded" audit type match, this method
         * will delegate to runtime-registered serializer (if any) through
         * the Audit::Serializer::JSONService class.
         */
        virtual void cannot_visit(const Tools::IVisitable &visitable) override;

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
}
}
