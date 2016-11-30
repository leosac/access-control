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

#include "SMTPConfig.hpp"

using namespace Leosac;
using namespace Leosac::Module::SMTP;

SMTPConfig::SMTPConfig()
    : id_(0)
{
}

void SMTPConfig::server_add(SMTPServerInfo smtps)
{
    servers_.push_back(smtps);
}

void SMTPConfig::server_clear()
{
    servers_.clear();
}

const std::vector<SMTPServerInfo> &SMTPConfig::servers() const
{
    return servers_;
}

SMTPConfigId SMTPConfig::id() const
{
    return id_;
}
