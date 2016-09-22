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

#include "api/WiegandCardCRUD.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

WiegandCardCRUD::WiegandCardCRUD(RequestContext ctx)
    : CRUDResourceHandler(ctx)
{
}

CRUDResourceHandlerUPtr WiegandCardCRUD::instanciate(RequestContext)
{
    return Leosac::Module::WebSockAPI::CRUDResourceHandlerUPtr();
}

std::vector<CRUDResourceHandler::ActionActionParam>
WiegandCardCRUD::required_permission(CRUDResourceHandler::Verb verb,
                                     const json &req) const
{
    return std::vector<CRUDResourceHandler::ActionActionParam>();
}

json WiegandCardCRUD::create_impl(const json &req)
{
    return Leosac::Module::WebSockAPI::json();
}

json WiegandCardCRUD::read_impl(const json &req)
{
    return Leosac::Module::WebSockAPI::json();
}

json WiegandCardCRUD::update_impl(const json &req)
{
    return Leosac::Module::WebSockAPI::json();
}

json WiegandCardCRUD::delete_impl(const json &req)
{
    return Leosac::Module::WebSockAPI::json();
}
