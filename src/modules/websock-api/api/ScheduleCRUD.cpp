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

#include "api/ScheduleCRUD.hpp"
#include "tools/ISchedule.hpp"
#include "tools/db/DBService.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

ScheduleCRUD::ScheduleCRUD(RequestContext ctx)
    : CRUDResourceHandler(ctx)
{
}

CRUDResourceHandlerUPtr ScheduleCRUD::instanciate(RequestContext ctx)
{
    auto instance = CRUDResourceHandlerUPtr(new ScheduleCRUD(ctx));

    return instance;
}

std::vector<CRUDResourceHandler::ActionActionParam>
ScheduleCRUD::required_permission(CRUDResourceHandler::Verb verb,
                                  const json &req) const
{
    std::vector<CRUDResourceHandler::ActionActionParam> ret;
    SecurityContext::GroupActionParam gap;
    try
    {
        gap.group_id = req.at("credential_id").get<Auth::GroupId>();
    }
    catch (std::out_of_range &e)
    {
        gap.group_id = 0;
    }
    switch (verb)
    {
    case Verb::READ:
        ret.push_back(std::make_pair(SecurityContext::Action::SCHEDULE_READ, gap));
        break;
    case Verb::CREATE:
        ret.push_back(std::make_pair(SecurityContext::Action::SCHEDULE_CREATE, gap));
        break;
    case Verb::UPDATE:
        ret.push_back(std::make_pair(SecurityContext::Action::SCHEDULE_UPDATE, gap));
        break;
    case Verb::DELETE:
        ret.push_back(std::make_pair(SecurityContext::Action::SCHEDULE_DELETE, gap));
        break;
    }
    return ret;
}

json ScheduleCRUD::create_impl(const json &req)
{
    json rep;
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    t.commit();
    return rep;
}

json ScheduleCRUD::read_impl(const json &req)
{
    json rep;

    return rep;
}

json ScheduleCRUD::update_impl(const json &)
{
    json rep;

    return rep;
}

json ScheduleCRUD::delete_impl(const json &)
{
    return {};
}
