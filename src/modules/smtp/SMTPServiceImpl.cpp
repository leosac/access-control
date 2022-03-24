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

#include "SMTPServiceImpl.hpp"
#include "SMTPModule.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::SMTP;

SMTPServiceImpl::SMTPServiceImpl(SMTPModule &m)
    : smtp_(m)
{
}

void SMTPServiceImpl::async_send(const MailInfo &mail)
{
    smtp_.async_send_mail(mail);
}

void SMTPServiceImpl::async_send_to_admin(const MailInfo &mail)
{
    auto mail_copy  = mail;
    char *mail_addr = getenv("LEOSAC_ADMIN_MAIL");
    if (mail_addr)
    {
        mail_copy.to.insert(mail_copy.to.begin(), std::string(mail_addr));
    }
    else
        WARN("Failed to add administrator as a recipient for  email titled "
             << Colorize::cyan(mail_copy.title)
             << " because no admin "
                "email is configured. Please set the "
             << Colorize::green("LEOSAC_ADMIN_MAIL") << " environment variable.");

    async_send(mail_copy);
}
