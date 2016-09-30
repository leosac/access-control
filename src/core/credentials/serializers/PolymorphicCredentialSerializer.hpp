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

#include "core/credentials/IWiegandCard.hpp"
#include "tools/Serializer.hpp"
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
    : public Serializer<json, Cred::ICredential, PolymorphicCredentialJSONSerializer>
{
    static json serialize(const Cred::ICredential &in, const SecurityContext &sc);

    static void unserialize(Cred::ICredential &out, const json &in,
                            const SecurityContext &sc);

  private:
    /**
     * Non static helper that can visit credential object.
     */
    struct Helper : public Tools::Visitor<Cred::IWiegandCard>
    {
        Helper(const SecurityContext &sc);
        void visit(const Cred::IWiegandCard &t) override;

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

/**
 *
 * Serialize the object as a JSON string.
 *
 * @see PolymorphicCredentialJSONSerializer
 */
struct PolymorphicCredentialJSONStringSerializer
    : public Serializer<std::string, Cred::ICredential,
                        PolymorphicCredentialJSONStringSerializer>
{
    static std::string serialize(const Cred::ICredential &in,
                                 const SecurityContext &sc);

    static void unserialize(Cred::ICredential &out, const std::string &in,
                            const SecurityContext &sc);
};
}
