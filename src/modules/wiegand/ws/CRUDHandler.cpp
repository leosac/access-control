/*
    Copyright (C) 2014-2017 Leosac

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

#include "modules/wiegand/ws/CRUDHandler.hpp"
#include "WiegandConfigSerializer.hpp"
#include "exception/EntityNotFound.hpp"
#include "modules/websock-api/api/APISession.hpp"
#include "modules/wiegand/WiegandConfig_odb.h"
#include "tools/JSONUtils.hpp"
#include "tools/db/DBService.hpp"
#include "tools/db/OptionalTransaction.hpp"

namespace Leosac
{
namespace Module
{
namespace Wiegand
{

std::vector<WebSockAPI::ICRUDResourceHandler::ActionActionParam>
CRUDHandler::required_permission(WebSockAPI::ICRUDResourceHandler::Verb verb,
                                 const json &req) const
{
    std::vector<CRUDResourceHandler::ActionActionParam> ret;

    SecurityContext::HardwareAuthSourceParam hardware_action_param;
    try
    {
        hardware_action_param.hardware_id =
            req.at("reader_id").get<WiegandReaderConfigId>();
    }
    catch (json::out_of_range &e)
    {
        hardware_action_param.hardware_id = 0;
    }

    switch (verb)
    {
    case Verb::READ:
        ret.emplace_back(SecurityContext::Action::HARDWARE_AUTH_SOURCE_READ,
                         hardware_action_param);
        break;
    case Verb::CREATE:
        ret.emplace_back(SecurityContext::Action::HARDWARE_AUTH_SOURCE_CREATE,
                         hardware_action_param);
        break;
    case Verb::UPDATE:
        ret.emplace_back(SecurityContext::Action::HARDWARE_AUTH_SOURCE_UPDATE,
                         hardware_action_param);
        break;
    case Verb::DELETE:
        ret.emplace_back(SecurityContext::Action::HARDWARE_AUTH_SOURCE_DELETE,
                         hardware_action_param);
        break;
    }
    return ret;
}

boost::optional<json> CRUDHandler::create_impl(const json &req)
{
    json rep;
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    auto new_reader = std::make_shared<WiegandReaderConfig>();
    WiegandReaderConfigSerializer::unserialize(*new_reader, req.at("attributes"),
                                               security_context());
    db->persist(new_reader);

    rep["data"] =
        WiegandReaderConfigSerializer::serialize(*new_reader, security_context());
    t.commit();
    return rep;
}

auto find_reader_by_id(const WiegandReaderConfigId &id, DBPtr db)
{
    db::OptionalTransaction t(db->begin());
    auto reader = db->find<WiegandReaderConfig>(id);
    t.commit();
    if (!reader)
        throw EntityNotFound(id, "wiegand-reader");
    return reader;
}

boost::optional<json> CRUDHandler::read_impl(const json &req)
{
    json rep;

    using Result = odb::result<WiegandReaderConfig>;
    DBPtr db     = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto reader_id = req.at("reader_id").get<WiegandReaderConfigId>();

    if (reader_id != 0)
    {
        auto reader = find_reader_by_id(reader_id, db);
        rep["data"] =
            WiegandReaderConfigSerializer::serialize(*reader, security_context());
    }
    else
    {
        Result result     = db->query<WiegandReaderConfig>();
        rep["data"]       = json::array();
        auto current_user = ctx_.session->current_user();

        // fixme: may be rather slow.
        for (const auto &reader : result)
        {
            SecurityContext::HardwareAuthSourceParam hap{.hardware_id = reader.id};
            if (ctx_.session->security_context().check_permission(
                    SecurityContext::Action::HARDWARE_AUTH_SOURCE_READ, hap))
            {
                rep["data"].push_back(WiegandReaderConfigSerializer::serialize(
                    reader, security_context()));
            }
        }
    }
    t.commit();
    return rep;
}

boost::optional<json> CRUDHandler::update_impl(const json &req)
{
    json rep;
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto reader_id = req.at("reader_id").get<WiegandReaderConfigId>();
    auto reader    = find_reader_by_id(reader_id, db);

    WiegandReaderConfigSerializer::unserialize(*reader, req.at("attributes"),
                                               security_context());

    db->update(reader);
    rep["data"] =
        WiegandReaderConfigSerializer::serialize(*reader, security_context());
    t.commit();
    return rep;
}

boost::optional<json> CRUDHandler::delete_impl(const json &req)
{
    auto did = req.at("reader_id").get<WiegandReaderConfigId>();
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    auto reader = find_reader_by_id(did, db);
    db->erase(reader);
    t.commit();

    return json{};
}

CRUDHandler::CRUDHandler(const WebSockAPI::RequestContext &ctx)
    : CRUDResourceHandler(ctx)
{
}

WebSockAPI::CRUDResourceHandlerUPtr
CRUDHandler::instanciate(WebSockAPI::RequestContext ctx)
{
    auto instance = WebSockAPI::CRUDResourceHandlerUPtr(new CRUDHandler(ctx));
    return instance;
}
}
}
}
