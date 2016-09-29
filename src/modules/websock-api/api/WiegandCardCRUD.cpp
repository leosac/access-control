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
#include "core/credentials/Credential.hpp"
#include "core/credentials/serializers/CredentialSerializer.hpp"
#include "core/credentials/serializers/PolymorphicCredentialSerializer.hpp"
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
    return {};
}

json WiegandCardCRUD::create_impl(const json &req)
{
    return Leosac::Module::WebSockAPI::json();
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
            rep["data"].push_back(PolymorphicCredentialJSONSerializer::serialize(
                cred, security_context()));
        }
    }
    t.commit();
    return rep;
}

json WiegandCardCRUD::update_impl(const json &req)
{
    return Leosac::Module::WebSockAPI::json();
}

json WiegandCardCRUD::delete_impl(const json &req)
{
    return Leosac::Module::WebSockAPI::json();
}
