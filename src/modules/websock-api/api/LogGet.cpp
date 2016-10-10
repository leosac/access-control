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

#include "LogGet.hpp"
#include "Exceptions.hpp"
#include "LogEntry_odb.h"
#include "api/APISession.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/db/DBService.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

LogGet::LogGet(RequestContext ctx)
    : MethodHandler(ctx)
{
}

MethodHandlerUPtr LogGet::create(RequestContext ctx)
{
    return std::make_unique<LogGet>(ctx);
}

json LogGet::process_impl(const json &req)
{
    json rep;
    DBPtr db = ctx_.dbsrv->db();
    if (db)
    {
        using namespace Tools;
        using namespace JSONUtil;

        rep["data"]      = json::array();
        std::string sort = extract_with_default(req, "sort", "desc");
        int p            = extract_with_default(req, "p", 0);   // page
        int ps           = extract_with_default(req, "ps", 20); // page size
        if (ps <= 0)
            ps = 1;

        LogEntry::QueryResult result = LogEntry::retrieve(db, p, ps, sort == "asc");
        for (Tools::LogEntry &entry : result.entries)
        {
            auto timestamp = boost::posix_time::to_time_t(entry.timestamp_);
            rep["data"].push_back(
                {{"id", entry.id_},
                 {"type", "log-message"},
                 {"attributes",
                  {{"message", entry.msg_}, {"timestamp", timestamp}}}});
        }

        rep["meta"] = {
            {"total", result.total}, {"last", result.last}, {"first", result.first}};
        rep["status"] = 0;
    }
    else
    {
        rep["status"] = -1;
    }
    return rep;
}

std::vector<MethodHandler::ActionActionParam>
LogGet::required_permission(const json &) const
{
    std::vector<MethodHandler::ActionActionParam> perm_;
    SecurityContext::ActionParam ap;

    perm_.push_back({SecurityContext::Action::LOG_READ, ap});
    return perm_;
}
