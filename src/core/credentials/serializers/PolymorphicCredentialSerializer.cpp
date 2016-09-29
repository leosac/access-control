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

#include "core/credentials/serializers/PolymorphicCredentialSerializer.hpp"
#include "WiegandCardSerializer.hpp"
#include "core/SecurityContext.hpp"
#include "core/credentials/ICredential.hpp"
#include "tools/JSONUtils.hpp"

using namespace Leosac;
using namespace Leosac::Cred;

json PolymorphicCredentialJSONSerializer::serialize(const ICredential &in,
                                                    const SecurityContext &sc)
{
    Helper h(sc);
    in.accept(h);
    return h.result_;
}

PolymorphicCredentialJSONSerializer::Helper::Helper(const SecurityContext &sc)
    : security_context_(sc)
{
}

void PolymorphicCredentialJSONSerializer::Helper::visit(const IWiegandCard &t)
{
    result_ = WiegandCardJSONSerializer::serialize(t, security_context_);
}
