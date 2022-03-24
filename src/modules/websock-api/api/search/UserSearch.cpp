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

#include "api/search/UserSearch.hpp"
#include "core/auth/User_odb.h"
#include "modules/websock-api/api/search/SearchBase.hpp"


using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

UserSearch::UserSearch(RequestContext ctx)
    : MethodHandler(ctx)
{
}

MethodHandlerUPtr UserSearch::create(RequestContext ctx)
{
    return std::make_unique<UserSearch>(ctx);
}

json UserSearch::process_impl(const json &req)
{
    return EntitySearchTool<Auth::User, use_username_tag>().search_json(
        ctx_.dbsrv->db(), req.at("partial_name").get<std::string>());
}

std::vector<ActionActionParam> UserSearch::required_permission(const json &) const
{
    std::vector<ActionActionParam> perm_;
    perm_.push_back({SecurityContext::Action::USER_SEARCH, {}});
    return perm_;
}
