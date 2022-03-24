/*
    Copyright (C) 2014-2022 Leosac

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
#include "tools/Mail.hpp"

namespace Leosac
{
namespace Module
{
namespace SMTP
{
class SMTPServiceImpl : public SMTPService
{
  public:
    SMTPServiceImpl(SMTPModule &);

    virtual void async_send(const MailInfo &mail) override;

    virtual void async_send_to_admin(const MailInfo &mail) override;

  private:
    SMTPModule &smtp_;
};
}
}
}
