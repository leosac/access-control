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
#include "core/credentials/IWiegandCard.hpp"
#include "tools/Visitor.hpp"
#include <json.hpp>
#include <string>

namespace Leosac
{
using json = nlohmann::json;

/**
 * A serializer that handle any subclass of ICredential.
 *
 * This credential, as opposed to CredentialJSONSerializer will fully serialize the
 * object.
 * This means that it will (try-to) find the correct serializer for the real type of
 * the object.
 */
struct PolymorphicCredentialJSONSerializer
{
    static json serialize(const Cred::ICredential &in, const SecurityContext &sc);

    static void unserialize(Cred::ICredential &out, const json &in,
                            const SecurityContext &sc);

    /**
     * Returns the "type-name" of the credential. This is something
     * that maps to EmberJS models.
     *
     * For example, for a wiegand card it would return "wiegand-card".
     */
    static std::string type_name(const Cred::ICredential &in);

  private:
    /**
     * Non static helper that can visit credential object.
     */
    struct HelperSerialize : public Tools::Visitor<Cred::IWiegandCard>,
                             public Tools::Visitor<Cred::IPinCode>
    {
        HelperSerialize(const SecurityContext &sc);
        void visit(const Cred::IWiegandCard &t) override;

        void visit(const Cred::IPinCode &t) override;

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

    struct HelperUnserialize : public Tools::Visitor<Cred::IWiegandCard>,
                               public Tools::Visitor<Cred::IPinCode>
    {
        HelperUnserialize(const SecurityContext &sc, const json &payload);
        void visit(Cred::IWiegandCard &t) override;

        void visit(Cred::IPinCode &t) override;

        /**
         * Reference to the security context.
         */
        const SecurityContext &security_context_;

        /**
         * The payload to unserialize from.
         */
        json payload_;
    };
};

/**
 *
 * Serialize the object as a JSON string.
 *
 * @see PolymorphicCredentialJSONSerializer
 */
struct PolymorphicCredentialJSONStringSerializer
{
    static std::string serialize(const Cred::ICredential &in,
                                 const SecurityContext &sc);

    static void unserialize(Cred::ICredential &out, const std::string &in,
                            const SecurityContext &sc);
};
}
