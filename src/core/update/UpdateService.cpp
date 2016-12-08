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
#include "tools/GenGuid.h"
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

    create_update_sig_.connect(
        CreateUpdateT::slot_type(&UpdateBackend::create_update, backend.get(),
                                 boost::placeholders::_1)
            .track_foreign(backend));

    ack_update_sig_.connect(AckUpdateT::slot_type(&UpdateBackend::ack_update,
                                                  backend.get(),
                                                  boost::placeholders::_1)
                                .track_foreign(backend));

    cancel_update_sig_.connect(
        CancelUpdateT::slot_type(&UpdateBackend::cancel_update, backend.get(),
                                 boost::placeholders::_1)
            .track_foreign(backend));
}

std::vector<UpdateDescriptorPtr> UpdateService::check_update()
{
    auto descriptors = check_update_sig_();

    // When a new check_update happens, we clear all
    // previous update descriptor as they would possibly
    // become outdated.
    published_descriptors_.clear();
    for (const auto &descriptor : descriptors)
    {
        // This is so a client can reference a descriptor later to
        // trigger creation of an update. (This is needed since we don't want
        // update descriptor to be database-persisted).
        published_descriptors_[descriptor->uuid] = descriptor;
    }

    return descriptors;
}

std::vector<IUpdatePtr> UpdateService::pending_updates()
{
    const auto &db = get_service_registry().get_service<DBService>()->db();
    db::OptionalTransaction t(db->begin());

    std::vector<IUpdatePtr> updates;
    using Query = odb::query<update::Update>;
    auto updates_odb =
        db->query<update::Update>(Query::status == update::Status::PENDING);

    for (auto i(updates_odb.begin()); i != updates_odb.end(); ++i)
    {
        IUpdatePtr ptr(i.load());
        ASSERT_LOG(ptr, "Loading failed, but object should already be loaded.");
        updates.push_back(ptr);
    }
    t.commit();
    return updates;
}

IUpdatePtr UpdateService::create_update(const std::string &update_descriptor_uuid)
{
    auto itr = published_descriptors_.find(update_descriptor_uuid);
    if (itr == published_descriptors_.end())
    {
        throw LEOSACException("UpdateDescriptor doesn't exist anymore.");
    }

    return create_update_sig_(*(itr->second));
}

void UpdateService::ack_update(IUpdatePtr update)
{
    ack_update_sig_(update);
}

void UpdateService::cancel_update(IUpdatePtr update)
{
    cancel_update_sig_(update);
}

UpdateDescriptor::UpdateDescriptor()
    : uuid(gen_uuid())
    , severity(Severity::NORMAL)
{
}
}
}
