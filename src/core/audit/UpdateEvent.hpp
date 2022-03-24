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

#pragma once

#include "AuditEntry.hpp"
#include "IUpdateEvent.hpp"

namespace Leosac
{
namespace Audit
{
/**
 * Provides an implementation of IUpdateEvent.
 */
#pragma db object polymorphic callback(odb_callback)
class UpdateEvent : virtual public IUpdateEvent, public AuditEntry
{
  private:
    UpdateEvent() = default;

    friend class Factory;

    static std::shared_ptr<UpdateEvent> create(const DBPtr &database,
                                               update::IUpdatePtr target_update,
                                               AuditEntryPtr parent);

  public:
    std::string generate_description() const override;

  public:
    virtual ~UpdateEvent() = default;

    static std::shared_ptr<UpdateEvent> create_empty();

    void target(update::IUpdatePtr u) override;

    update::UpdateId target_id() const override;

  public:
    update::UpdateLWPtr target_;

    friend class odb::access;
};
}
}

#ifdef ODB_COMPILER
#include "core/update/Update.hpp"
#endif
