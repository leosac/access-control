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

#include "api/search/ScheduleSearch.hpp"
#include "api/search/SearchBase.hpp"
#include "tools/Schedule_odb.h"


using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

ScheduleSearch::ScheduleSearch(RequestContext ctx)
    : MethodHandler(ctx)
{
}

MethodHandlerUPtr ScheduleSearch::create(RequestContext ctx)
{
    return std::make_unique<ScheduleSearch>(ctx);
}

json ScheduleSearch::process_impl(const json &req)
{
    return EntitySearchTool<Tools::Schedule, use_name_tag>().search_json(
        ctx_.dbsrv->db(), req.at("partial_name").get<std::string>());
}

std::vector<ActionActionParam>
ScheduleSearch::required_permission(const json &) const
{
    std::vector<ActionActionParam> perm_;
    perm_.push_back({SecurityContext::Action::SCHEDULE_SEARCH, {}});
    return perm_;
}
