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

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::SMTP;

json SMTPServerInfoJSONSerializer::serialize(const SMTPServerInfo &in,
                                             const SecurityContext &)
{
    json server_desc;
    server_desc["url"]         = in.url_;
    server_desc["from"]        = in.from_;
    server_desc["username"]    = in.username_;
    server_desc["verify_host"] = in.verify_host_;
    server_desc["verify_peer"] = in.verify_peer_;

    return server_desc;
}

void SMTPServerInfoJSONSerializer::unserialize(SMTPServerInfo &out, const json &in,
                                               const SecurityContext &)
{
    out.url_      = in.at("url");
    out.from_     = in.at("from");
    out.username_ = in.at("username");

    if (in.count("verify_peer"))
        out.verify_peer_ = in.at("verify_peer");
    if (in.count("verify_host"))
        out.verify_host_ = in.at("verify_host");
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
