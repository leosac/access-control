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

#include "modules/auth/auth-db/AuthDBModule.hpp"
#include "core/CoreUtils.hpp"
#include "core/kernel.hpp"
#include <tools/db/database.hpp>

using namespace Leosac;
using namespace Leosac::Module::Auth;

AuthDBModule::AuthDBModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                           const boost::property_tree::ptree &cfg,
                           CoreUtilsPtr utils)
    : AsioModule(ctx, pipe, cfg, utils)
{
    process_config();

    /*   for (auto authenticator : authenticators_)
       {
           reactor_.add(authenticator->bus_sub(),
                        std::bind(&AuthDBInstance::handle_bus_msg, authenticator));
       }*/
}

AuthDBModule::~AuthDBModule()
{
}

void AuthDBModule::process_config()
{
    setup_database();
}

void AuthDBModule::on_service_event(const service_event::Event &event)
{
}

void AuthDBModule::setup_database()
{
    using namespace odb;
    using namespace odb::core;
    auto db          = utils_->database();
    schema_version v = db->schema_version("module_auth-db");
    schema_version cv(schema_catalog::current_version(*db, "module_auth-db"));
    if (v == 0)
    {
        transaction t(db->begin());
        schema_catalog::create_schema(*db, "module_auth-db");
        t.commit();
    }
    else if (v < cv)
    {
        INFO("AuthDB Module performing database migration. Going from version "
             << v << " to version " << cv);
        transaction t(db->begin());
        schema_catalog::migrate(*db, cv, "module_auth-db");
        t.commit();
    }
}
