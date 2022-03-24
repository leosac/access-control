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

#include "api/search/CredentialSearch.hpp"
#include "Exceptions.hpp"
#include "api/APISession.hpp"
#include "core/credentials/Credential_odb.h"
#include "tools/db/DBService.hpp"
#include "tools/log.hpp"
#include <core/credentials/serializers/PolymorphicCredentialSerializer.hpp>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

CredentialSearch::CredentialSearch(RequestContext ctx)
    : MethodHandler(ctx)
{
}

MethodHandlerUPtr CredentialSearch::create(RequestContext ctx)
{
    return std::make_unique<CredentialSearch>(ctx);
}

struct CredentialComparator
{
    bool operator()(const Cred::CredentialPtr &c1, const Cred::CredentialPtr &c2)
    {
        ASSERT_LOG(c1, "Credential c1 is null");
        ASSERT_LOG(c2, "Credential c2 is null");
        ASSERT_LOG(c1->id(), "c1 has no id.");
        ASSERT_LOG(c2->id(), "c2 has no id.");
        return c1->id() < c2->id();
    }
};

json CredentialSearch::process_impl(const json &req)
{
    json rep = json::array();
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    using Query              = odb::query<Cred::Credential>;
    std::string partial_name = req.at("partial_name");
    std::set<Cred::CredentialPtr, CredentialComparator> creds;

    // We want a case insensitive search. However, there is no portable
    // way to do this. So for now we'll rely on a naive, potentially slow
    // bruteforce-style implementation.
    // todo: fixme

    for (auto i = 0u; i < partial_name.length(); ++i)
    {
        auto partial_name_copy = partial_name;
        partial_name_copy[i]   = std::toupper(partial_name[i]);
        Query q(Query::alias.like("%" + partial_name_copy + "%"));
        auto results = db->query(q);
        for (const auto &cred : results)
        {
            // We want shared_ptr to store in set. Call load, this should load from
            // the cache anyway.
            creds.insert(db->load<Cred::Credential>(cred.id()));
        }
    }

    Query q(Query::alias.like("%" + partial_name + "%"));
    auto results = db->query(q);
    for (const auto &cred : results)
    {
        // We want shared_ptr to store in set. Call load, this should load from the
        // cache anyway.
        creds.insert(db->load<Cred::Credential>(cred.id()));
    }

    for (const auto &cred : creds)
    {
        ASSERT_LOG(cred, "Credential is null.");
        json result_json = {
            {"id", cred->id()},
            {"alias", cred->alias()},
            {"type", PolymorphicCredentialJSONSerializer::type_name(*cred)}};
        rep.push_back(result_json);
    }

    return rep;
}

std::vector<ActionActionParam>
CredentialSearch::required_permission(const json &) const
{
    std::vector<ActionActionParam> perm_;
    SecurityContext::ActionParam ap{};

    perm_.push_back({SecurityContext::Action::DOOR_SEARCH, ap});
    return perm_;
}
