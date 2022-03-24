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

#include "Exceptions.hpp"
#include "core/auth/Token.hpp"
#include "core/auth/User.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

SessionAborted::SessionAborted()
    : LEOSACException("Websocket session has been aborted.")
{
}

SessionAborted::SessionAborted(Auth::TokenPtr token)
    : LEOSACException(build_msg(token))
{
}

std::string SessionAborted::build_msg(Auth::TokenPtr token) const
{
    std::stringstream ss;
    ss << "Websocket session has been aborted.";

    if (!token)
        ss << "No associated token found.";
    else
    {
        ss << " Token " << token->token();
        if (token->owner())
            ss << ", owned by user " << token->owner()->id();
        else
            ss << ", with no owner ";
        ss << " expired on " << token->expiration();
    }
    return ss.str();
}

MalformedMessage::MalformedMessage(const std::string &detail)
    : LEOSACException(build_msg(detail))
{
}

std::string MalformedMessage::build_msg(const std::string &detail) const
{
    std::stringstream ss;
    ss << "Malformed message";

    if (detail.size())
        ss << ": " << detail;
    else
        ss << ".";

    return ss.str();
}
