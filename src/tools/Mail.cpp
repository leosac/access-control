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

#include "tools/Mail.hpp"
#include "GenGuid.h"
#include "core/CoreUtils.hpp"
#include "core/Scheduler.hpp"
#include "core/tasks/GenericTask.hpp"
#include "tools/registry/GlobalRegistry.hpp"

using namespace Leosac;

MailSender::MailSender(CoreUtilsPtr utils)
    : utils_(utils)
{
    ASSERT_LOG(utils, "CoreUtils pointer is null.");
}

void MailSender::send(const MailInfo &mail)
{
    // Since we wont wait for task completion, we have to copy the mail object.
    auto task = Tasks::GenericTask::build([ utils = utils_, mail = mail ]() {
        // We'll store the MailInfo structure in the GlobalRegistry and pass
        // the key to the mailer service. It'll then be able to retrieve the
        // MailInfo object and process the email.

        zmqpp::socket sock(utils->zmqpp_context(), zmqpp::socket_type::push);
        sock.connect("inproc://zmq-bus-pull");

        zmqpp::message msg;
        msg << "SERVICE.MAILER"; // The topic the SMTP module listen on.

        auto expire =
            GlobalRegistry::Clock::now() + std::chrono::milliseconds(10000);
        auto key = gen_uuid();
        GlobalRegistry::set(key, mail, expire);
        msg << key;
        sock.send(msg);

        return true;
    });
    utils_->scheduler().enqueue(task, TargetThread::POOL);
}

void MailSender::send_to_admin(const MailInfo &mail)
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

    send(mail_copy);
}
