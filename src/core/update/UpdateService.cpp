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

#include "UpdateService.hpp"
#include "Update_odb.h"
#include "core/GetServiceRegistry.hpp"
#include "tools/db/OptionalTransaction.hpp"

namespace Leosac
{
namespace update
{
void UpdateService::register_backend(UpdateBackendPtr backend)
{
    check_update_sig_.connect(
        CheckUpdateT::slot_type(&UpdateBackend::check_update, backend.get())
            .track_foreign(backend));
}

void UpdateService::check_update()
{
    check_update_sig_();
}

std::vector<IUpdatePtr> UpdateService::pending_updates()
{
    const auto &db = get_service_registry().get_service<DBService>()->db();
    db::OptionalTransaction t(db->begin());

    std::vector<IUpdatePtr> updates;
    auto updates_odb = db->query<Update>();
    for (const auto &update : updates)
    {
        updates.push_back(update);
    }
    t.commit();
    return updates;
}
}
}
