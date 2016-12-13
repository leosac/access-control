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
#include "tools/db/database.hpp"

#pragma db model version(1, 1, open)

namespace Leosac
{
namespace Module
{
namespace SMTP
{

#pragma db value
struct SMTPServerInfo
{
    SMTPServerInfo()
        : ms_timeout(30 * 1000)
        , verify_host(false)
        , verify_peer(false)
        , enabled(true){};

    SMTPServerInfo(const SMTPServerInfo &) = default;

    std::string url;
    std::string from;
    std::string username;
    std::string password;

    /**
     * Timeout in millisecond for an operation against
     * this server. Defaults to 30s.
     */
    int ms_timeout;

    bool verify_host;
    bool verify_peer;
    std::string CA_info_file_;

    /**
     * Should this server be used to send email ?
     */
    bool enabled;
};

/**
 * Wrapper around the SMTP module configuration.
 * There probably shouldn't be more than one object
 * of this type in the database.
 */
#pragma db object table("SMTP_Config")
class SMTPConfig
{
  public:
    SMTPConfig();

    SMTPConfigId id() const;

    void server_add(SMTPServerInfo);
    void server_clear();
    const std::vector<SMTPServerInfo> &servers() const;

  private:
#pragma db id auto
    SMTPConfigId id_;

#pragma db value_not_null id_column("smtpconfig_id")
    std::vector<SMTPServerInfo> servers_;

    friend class odb::access;
};
}
}
}
