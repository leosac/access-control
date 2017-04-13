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

#include "modules/ws2/Parser.hpp"
#include <json.hpp>

namespace Leosac
{
namespace Module
{
namespace WS2
{
ClientMessage Parser::parse(const Parser::MessagePtr &msg)
{
    ClientMessage out;
    json req = json::parse(msg->get_payload());

    try
    {
        // Extract general message argument.
        out.uuid    = req.at("uuid");
        out.type    = req.at("type");
        out.content = req.at("content");
    }
    catch (const std::out_of_range &e)
    {
        throw std::runtime_error("Malformed");
        // throw MalformedMessage();
    }
    catch (const std::domain_error &e)
    {
        throw std::runtime_error("Malformed");
        // throw MalformedMessage();
    }
    return out;
}
}
}
}
