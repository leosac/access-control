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

#include "modules/smtp/SMTPServerInfoSerializer.hpp"
#include "modules/smtp/SMTPConfig.hpp"
#include "tools/JSONUtils.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::SMTP;

json SMTPServerInfoJSONSerializer::serialize(const SMTPServerInfo &in,
                                             const SecurityContext &)
{
    json server_desc;
    server_desc["url"]      = in.url;
    server_desc["from"]     = in.from;
    server_desc["username"] = in.username;
    server_desc["password"] = in.password;

    server_desc["timeout"]     = in.ms_timeout;
    server_desc["verify_host"] = in.verify_host;
    server_desc["verify_peer"] = in.verify_peer;

    server_desc["enabled"] = in.enabled;
    return server_desc;
}

void SMTPServerInfoJSONSerializer::unserialize(SMTPServerInfo &out, const json &in,
                                               const SecurityContext &)
{
    using namespace JSONUtil;

    out.url      = extract_with_default(in, "url", out.url);
    out.from     = extract_with_default(in, "from", out.from);
    out.username = extract_with_default(in, "username", out.username);
    out.password = extract_with_default(in, "password", out.password);

    out.ms_timeout  = extract_with_default(in, "timeout", out.ms_timeout);
    out.verify_peer = extract_with_default(in, "verify_peer", out.verify_peer);
    out.verify_host = extract_with_default(in, "verify_host", out.verify_host);

    out.enabled = extract_with_default(in, "enabled", out.enabled);
}

std::string SMTPServerInfoJSONStringSerializer::serialize(const SMTPServerInfo &in,
                                                          const SecurityContext &sc)
{
    return SMTPServerInfoJSONSerializer::serialize(in, sc).dump(4);
}

void SMTPServerInfoJSONStringSerializer::unserialize(SMTPServerInfo &out,
                                                     const std::string &in,
                                                     const SecurityContext &sc)
{
    return SMTPServerInfoJSONSerializer::unserialize(out, json::parse(in), sc);
}
