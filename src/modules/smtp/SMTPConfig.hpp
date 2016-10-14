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

#include "SMTPFwd.hpp"
#include "tools/db/database.hpp"

namespace Leosac
{
namespace Module
{
namespace SMTP
{
#pragma db value

struct SMTPServerInfo
{
    SMTPServerInfo() = default;

    SMTPServerInfo(const SMTPServerInfo &) = default;

    std::string url_;

    std::string from_;

    std::string username_;

    std::string password_;

    bool verify_host_;
    bool verify_peer_;
    std::string CA_info_file_;
};

/**
 * Wrapper around the SMTP module configuration.
 * There probably shouldn't be more than one object
 * of this type in the database.
 */
#pragma db object

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
