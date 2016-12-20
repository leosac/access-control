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

#include "api/CredentialCRUD.hpp"
#include "Credential_odb.h"
#include "core/audit/AuditFactory.hpp"
#include "core/audit/ICredentialEvent.hpp"
#include "core/credentials/Credential.hpp"
#include "core/credentials/PinCode.hpp"
#include "core/credentials/RFIDCard.hpp"
#include "core/credentials/serializers/CredentialSerializer.hpp"
#include "core/credentials/serializers/PolymorphicCredentialSerializer.hpp"
#include "exception/leosacexception.hpp"
#include "tools/AssertCast.hpp"
#include "tools/db/DBService.hpp"
#include "tools/registry/ThreadLocalRegistry.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

CredentialCRUD::CredentialCRUD(RequestContext ctx)
    : CRUDResourceHandler(ctx)
{
}

CRUDResourceHandlerUPtr CredentialCRUD::instanciate(RequestContext ctx)
{
    auto instance = CRUDResourceHandlerUPtr(new CredentialCRUD(ctx));

    return instance;
}

std::vector<CRUDResourceHandler::ActionActionParam>
CredentialCRUD::required_permission(CRUDResourceHandler::Verb verb,
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

boost::optional<json> CredentialCRUD::create_impl(const json &req)
{
    json rep;
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    Cred::ICredentialPtr new_cred;
    std::string type = req.at("credential-type");
    if (type == "rfid-card")
    {
        new_cred = std::make_shared<Cred::RFIDCard>();
    }
    else if (type == "pin-code")
    {
        new_cred = std::make_shared<Cred::PinCode>();
    }
    else
    {
        throw LEOSACException(
            BUILD_STR("Credential {" << type << "} are not supported."));
    }

    PolymorphicCredentialJSONSerializer::unserialize(*new_cred, req.at("attributes"),
                                                     security_context());
    db->persist(assert_cast<Cred::CredentialPtr>(new_cred));
    Audit::ICredentialEventPtr audit =
        Audit::Factory::CredentialEventPtr(db, new_cred, ctx_.audit);
    audit->event_mask(Audit::EventType::CREDENTIAL_CREATED);
    audit->after(PolymorphicCredentialJSONStringSerializer::serialize(
        *new_cred, SystemSecurityContext::instance()));
    audit->finalize();

    rep["data"] = PolymorphicCredentialJSONSerializer::serialize(*new_cred,
                                                                 security_context());

    t.commit();
    return rep;
}

boost::optional<json> CredentialCRUD::read_impl(const json &req)
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
    }
    t.commit();
    return rep;
}

boost::optional<json> CredentialCRUD::update_impl(const json &req)
{
    json rep;
    auto cid = req.at("credential_id").get<Cred::CredentialId>();
    auto db  = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    Cred::ICredentialPtr cred =
        ctx_.dbsrv->find_credential_by_id(cid, DBService::THROW_IF_NOT_FOUND);
    Audit::ICredentialEventPtr audit =
        Audit::Factory::CredentialEventPtr(db, cred, ctx_.audit);
    audit->event_mask(Audit::EventType::CREDENTIAL_UPDATED);
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

boost::optional<json> CredentialCRUD::delete_impl(const json &req)
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
        audit->event_mask(Audit::EventType::CREDENTIAL_DELETED);
        audit->before(PolymorphicCredentialJSONStringSerializer::serialize(
            *cred, SystemSecurityContext::instance()));

        audit->finalize();
        db->erase<Cred::Credential>(cred->id());
        t.commit();
    }
    return json{};
}
