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

#include "SMTPAudit.hpp"
#include "core/audit/IAuditEntry.hpp"
#include "tools/Serializer.hpp"
#include "tools/Visitor.hpp"
#include <json.hpp>
#include <string>

namespace Leosac
{
using json = nlohmann::json;

namespace Module
{
namespace SMTP
{
struct SMTPAuditSerializer : public Serializer<json, SMTPAudit, SMTPAuditSerializer>
{
    static json serialize(const SMTPAudit &in, const SecurityContext &sc);

    /**
     * A one-time-use helper that will visit IAuditEntry object and call
     * serializer if its an SMTPAudit.
     *
     * This object is used by the dynamically-registered serializer for SMTPAudit.
     */
    struct Helper : public Tools::Visitor<SMTPAudit>
    {
        Helper(const SecurityContext &sc);
        virtual void cannot_visit(const Tools::IVisitable &visitable) override;

        virtual void visit(const SMTPAudit &t) override;

        /**
         * A flag to track wether or not we visited the an object.
         */
        bool has_visited_;
        json result_;
        const SecurityContext &sc_;
    };
};
}
}
}
