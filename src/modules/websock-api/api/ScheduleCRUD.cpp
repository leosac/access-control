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

#include "tools/Schedule.hpp"
#include "Schedule_odb.h"
#include "api/ScheduleCRUD.hpp"
#include "core/audit/AuditFactory.hpp"
#include "core/audit/IScheduleEvent.hpp"
#include "tools/AssertCast.hpp"
#include "tools/ISchedule.hpp"
#include "tools/db/DBService.hpp"
#include "tools/serializers/ScheduleSerializer.hpp"
#include <tools/serializers/ScheduleMappingSerializer.hpp>

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
    SecurityContext::ScheduleActionParam sap;
    try
    {
        sap.schedule_id = req.at("schedule_id").get<Auth::GroupId>();
    }
    catch (std::out_of_range &e)
    {
        sap.schedule_id = 0;
    }
    switch (verb)
    {
    case Verb::READ:
        ret.push_back(std::make_pair(SecurityContext::Action::SCHEDULE_READ, sap));
        break;
    case Verb::CREATE:
        ret.push_back(std::make_pair(SecurityContext::Action::SCHEDULE_CREATE, sap));
        break;
    case Verb::UPDATE:
        ret.push_back(std::make_pair(SecurityContext::Action::SCHEDULE_UPDATE, sap));
        break;
    case Verb::DELETE:
        ret.push_back(std::make_pair(SecurityContext::Action::SCHEDULE_DELETE, sap));
        break;
    }
    return ret;
}

boost::optional<json> ScheduleCRUD::create_impl(const json &req)
{
    json rep;
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    Tools::SchedulePtr schedule = std::make_shared<Tools::Schedule>();
    Tools::ScheduleJSONSerializer::unserialize(*schedule, req.at("attributes"),
                                               security_context());

    db->persist(schedule);
    Audit::IScheduleEventPtr audit =
        Audit::Factory::ScheduleEvent(db, schedule, ctx_.audit);
    audit->event_mask(Audit::EventType::SCHEDULE_CREATED);
    audit->after(Tools::ScheduleJSONStringSerializer::serialize(
        *schedule, SystemSecurityContext::instance()));
    audit->finalize();
    rep["data"] =
        Tools::ScheduleJSONSerializer::serialize(*schedule, security_context());
    t.commit();
    return rep;
}

/**
 * Returns a JSON array that describes the schedule-mapping objects
 * associated with the schedule.
 *
 * This is used to populate the "included" key (emberjs data sideloading)
 *
 * @param dest The json array push into.
 */
static void include_schedule_mapping_infos(json &dest,
                                           const Tools::ISchedule &schedule,
                                           SecurityContext &sc)
{
    ASSERT_LOG(dest.is_array(), "Destination is not array.");
    for (const auto &mapping : schedule.mapping())
    {
        dest.push_back(
            Tools::ScheduleMappingJSONSerializer::serialize(*mapping, sc));
    }
}

boost::optional<json> ScheduleCRUD::read_impl(const json &req)
{
    json rep;

    using Result = odb::result<Tools::Schedule>;
    DBPtr db     = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    auto sid        = req.at("schedule_id").get<Tools::ScheduleId>();
    rep["included"] = json::array();

    if (sid != 0)
    {
        Tools::ISchedulePtr schedule =
            ctx_.dbsrv->find_schedule_by_id(sid, DBService::THROW_IF_NOT_FOUND);
        rep["data"] =
            Tools::ScheduleJSONSerializer::serialize(*schedule, security_context());
        include_schedule_mapping_infos(rep["included"], *schedule,
                                       security_context());
    }
    else
    {
        Result result = db->query<Tools::Schedule>();
        rep["data"]   = json::array();
        for (const auto &schedule : result)
        {
            rep["data"].push_back(Tools::ScheduleJSONSerializer::serialize(
                schedule, security_context()));
            include_schedule_mapping_infos(rep["included"], schedule,
                                           security_context());
        }
    }
    return rep;
}

boost::optional<json> ScheduleCRUD::update_impl(const json &req)
{
    json rep;
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto sid = req.at("schedule_id").get<Tools::ScheduleId>();

    rep["included"] = json::array(); // Will contain ScheduleMapping data.

    auto schedule =
        ctx_.dbsrv->find_schedule_by_id(sid, DBService::THROW_IF_NOT_FOUND);
    Audit::IScheduleEventPtr audit =
        Audit::Factory::ScheduleEvent(db, schedule, ctx_.audit);
    audit->event_mask(Audit::EventType::SCHEDULE_UPDATED);
    audit->before(Tools::ScheduleJSONStringSerializer::serialize(
        *schedule, SystemSecurityContext::instance()));

    Tools::ScheduleJSONSerializer::unserialize(*schedule, req.at("attributes"),
                                               security_context());

    for (const auto &mapping : schedule->mapping())
    {
        db->erase(mapping);
    }
    schedule->clear_mapping();
    // Extract mappings.
    for (const auto &json_mapping : req.at("mapping"))
    {
        Tools::ScheduleMappingPtr mapping =
            std::make_shared<Tools::ScheduleMapping>();
        Tools::ScheduleMappingJSONSerializer::unserialize(*mapping, json_mapping,
                                                          security_context());
        db->persist(mapping);
        schedule->add_mapping(mapping);
    }
    include_schedule_mapping_infos(rep["included"], *schedule, security_context());

    db->update(assert_cast<Tools::SchedulePtr>(schedule));
    audit->after(Tools::ScheduleJSONStringSerializer::serialize(
        *schedule, SystemSecurityContext::instance()));
    audit->finalize();

    rep["data"] =
        Tools::ScheduleJSONSerializer::serialize(*schedule, security_context());
    t.commit();
    return rep;
}

boost::optional<json> ScheduleCRUD::delete_impl(const json &req)
{
    auto sid = req.at("schedule_id").get<Tools::ScheduleId>();
    auto db  = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    if (sid != 0)
    {
        auto schedule =
            ctx_.dbsrv->find_schedule_by_id(sid, DBService::THROW_IF_NOT_FOUND);
        Audit::IScheduleEventPtr audit =
            Audit::Factory::ScheduleEvent(db, schedule, ctx_.audit);

        audit->event_mask(Audit::EventType::SCHEDULE_DELETED);
        audit->before(Tools::ScheduleJSONStringSerializer::serialize(
            *schedule, SystemSecurityContext::instance()));

        for (const auto &mapping : schedule->mapping())
        {
            db->erase(mapping);
        }

        audit->finalize();
        db->erase<Tools::Schedule>(schedule->id());
        t.commit();
    }
    return {};
}
