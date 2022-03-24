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

#include "LeosacFwd.hpp"
#include <string>
#include <vector>
#include <zmqpp/zmqpp.hpp>

namespace Leosac
{

struct MailInfo
{
    std::vector<std::string> to;
    std::string title;
    std::string body;
};

/**
 * Reference interface for SMTP module.
 *
 * @note It is expected that the service implementation
 * is non-blocking and asynchronous.
 *
 * @note All methods in this interface MUST be thread safe.
 */
class SMTPService
{
  public:
    virtual ~SMTPService() = default;

    /**
     * Send a message to the mailer service (if any) to enqueue
     * an email.
     *
     * This method doesn't block: it doesn't wait for mail acknowledgment
     * that the mail as been queue, nor does it wait for the ZeroMQ connection
     * to the message bus.
     */
    virtual void async_send(const MailInfo &mail) = 0;

    /**
     * Similar to send, but add the leosac administrator email to the
     * recipients' list.
     *
     * The admin email is fetched from the environment: LEOSAC_ADMIN_MAIL
     */
    virtual void async_send_to_admin(const MailInfo &mail) = 0;
};
}
