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
#include "Credential_odb.h"
#include "core/audit/AuditFactory.hpp"
#include "core/audit/ICredentialEvent.hpp"
#include "core/credentials/Credential.hpp"
#include "core/credentials/serializers/CredentialSerializer.hpp"
#include "core/credentials/serializers/PolymorphicCredentialSerializer.hpp"
#include "tools/AssertCast.hpp"
#include "tools/GlobalRegistry.hpp"
#include "tools/db/DBService.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

WiegandCardCRUD::WiegandCardCRUD(RequestContext ctx)
    : CRUDResourceHandler(ctx)
{
}

CRUDResourceHandlerUPtr WiegandCardCRUD::instanciate(RequestContext ctx)
{
    auto instance = CRUDResourceHandlerUPtr(new WiegandCardCRUD(ctx));

    return instance;
}

std::vector<CRUDResourceHandler::ActionActionParam>
WiegandCardCRUD::required_permission(CRUDResourceHandler::Verb verb,
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
        ret.push_back(std::make_pair(SecurityContext::Action::CREDENTIAL_READ, gap));
        break;
    case Verb::CREATE:
        ret.push_back(
            std::make_pair(SecurityContext::Action::CREDENTIAL_CREATE, gap));
        break;
    case Verb::UPDATE:
        ret.push_back(
            std::make_pair(SecurityContext::Action::CREDENTIAL_UPDATE, gap));
        break;
    case Verb::DELETE:
        ret.push_back(
            std::make_pair(SecurityContext::Action::CREDENTIAL_DELETE, gap));
        break;
    }
    return ret;
}

json WiegandCardCRUD::create_impl(const json &req)
{
    json rep;

    return rep;
}

json WiegandCardCRUD::read_impl(const json &req)
{
    json rep;

    using Result = odb::result<Cred::Credential>;
    DBPtr db     = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto cid = req.at("credential_id").get<Auth::UserId>();

    if (cid != 0)
    {
        Cred::ICredentialPtr cred =
            ctx_.dbsrv->find_credential_by_id(cid, DBService::THROW_IF_NOT_FOUND);
        rep["data"] = PolymorphicCredentialJSONSerializer::serialize(
            *cred, security_context());
    }
    else
    {
        Result result = db->query<Cred::Credential>();
        rep["data"]   = json::array();
        for (const auto &cred : result)
        {
            if (security_context().check_permission(
                    SecurityContext::Action::CREDENTIAL_READ,
                    SecurityContext::CredentialActionParam{.credential_id =
                                                               cred.id()}))
            {
                rep["data"].push_back(PolymorphicCredentialJSONSerializer::serialize(
                    cred, security_context()));
            }
        }
        auto dummy                   = json{};
        dummy["type"]                = "pin-code";
        dummy["id"]                  = 42;
        dummy["attributes"]          = json::object();
        dummy["attributes"]["alias"] = "BOAP";
        dummy["attributes"]["code"]  = "1234";
        rep["data"].push_back(dummy);
    }
    t.commit();
    return rep;
}

json WiegandCardCRUD::update_impl(const json &req)
{
    json rep;
    auto cid = req.at("credential_id").get<Cred::CredentialId>();
    auto db  = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    Cred::ICredentialPtr cred =
        ctx_.dbsrv->find_credential_by_id(cid, DBService::THROW_IF_NOT_FOUND);
    Audit::ICredentialEventPtr audit =
        Audit::Factory::CredentialEventPtr(db, cred, ctx_.audit);
    audit->event_mask(Audit::EventType::CREDENTIAL_UPDATE);
    audit->before(PolymorphicCredentialJSONStringSerializer::serialize(
        *cred, SystemSecurityContext::instance()));

    PolymorphicCredentialJSONSerializer::unserialize(*cred, req.at("attributes"),
                                                     security_context());
    db->update(assert_cast<Cred::CredentialPtr>(cred));

    audit->after(PolymorphicCredentialJSONStringSerializer::serialize(
        *cred, SystemSecurityContext::instance()));
    audit->finalize();
    t.commit();
    return rep;
}

json WiegandCardCRUD::delete_impl(const json &req)
{
    auto cid = req.at("credential_id").get<Cred::CredentialId>();
    auto db  = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    if (cid != 0)
    {
        Cred::ICredentialPtr cred =
            ctx_.dbsrv->find_credential_by_id(cid, DBService::THROW_IF_NOT_FOUND);
        Audit::ICredentialEventPtr audit =
            Audit::Factory::CredentialEventPtr(db, cred, ctx_.audit);
        audit->event_mask(Audit::EventType::CREDENTIAL_DELETE);
        audit->before(PolymorphicCredentialJSONStringSerializer::serialize(
            *cred, SystemSecurityContext::instance()));

        audit->finalize();
        db->erase<Cred::Credential>(cred->id());
        t.commit();
    }
    return {};
}
