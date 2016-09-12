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

#include "WSAPICall.hpp"
#include "WSAPICall_odb.h"
#include "tools/db/MultiplexedTransaction.hpp"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Audit;

WSAPICall::WSAPICall()
    : status_code_(APIStatusCode::UNKNOWN)
{
}

WSAPICallPtr WSAPICall::create(const DBPtr &database)
{
    ASSERT_LOG(database, "Database cannot be null.");

    db::MultiplexedTransaction t(database->begin());
    WSAPICallPtr audit(new Audit::WSAPICall());
    database->persist(audit);
    t.commit();
    audit->database_ = database;
    return audit;
}

void WSAPICall::method(const std::string &str)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    api_method_ = str;
}

void WSAPICall::uuid(const std::string &str)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    uuid_ = str;
}

void WSAPICall::status_code(APIStatusCode code)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    status_code_ = code;
}

void WSAPICall::status_string(const std::string &str)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    status_string_ = str;
}

void WSAPICall::source_endpoint(const std::string &str)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    source_endpoint_ = str;
}

void WSAPICall::request_content(const std::string &str)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    request_content_ = str;
}

void WSAPICall::response_content(const std::string &str)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    response_content_ = str;
}
