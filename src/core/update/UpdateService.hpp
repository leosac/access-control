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

#pragma once

#include "UpdateFwd.hpp"
#include "core/SecurityContext.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/bs2.hpp"
#include "tools/registry/Registry.hpp"
#include "tools/serializers/ExtensibleSerializer.hpp"
#include <mutex>

namespace Leosac
{
namespace update
{
enum class Severity
{
    LOW      = 0,
    NORMAL   = 1,
    HIGHT    = 2,
    CRITICAL = 3,
};

/**
 * Describe an update that has yet to be done.
 *
 * This object is the format used to let end-user
 * knows that update are available. It stores which
 * module is the source of the potential update, aswel
 * as a quick description of the update itself.
 *
 * @note Update descriptor are kept alive in memory by the
 * UpdateService.
 */
struct UpdateDescriptor
{
    UpdateDescriptor();
    virtual ~UpdateDescriptor() = default;

    std::string uuid;
    Severity severity;
    std::string source_module;
    std::string update_desc;

  private:
    /**
     * Just to make UpdateDescriptor abstract so module have to provide their own
     * subclass. (needed for dynamic cast in create_update)
     */
    virtual void implement_me_() = 0;
};

class UpdateBackend
{
  public:
    /**
     * Check for updates against arbitrary, module-owned object.
     *
     * The update returned by check update are supposed to be
     * transient. They are informative only and simply indicates
     * that an update is needed.
     *
     * Module specific websocket-handler should be called in order
     * to to perform other operation wrt updates.
     */
    virtual std::vector<UpdateDescriptorPtr> check_update() = 0;

    /**
     * Create an update based on the UpdateDescriptor `ud`.
     *
     * If the `ud` cannot be casted back to the implementation of your
     * module, make sure return nullptr from this method because it
     * means the update creation wasn't targeted at your module.
     */
    virtual IUpdatePtr create_update(const UpdateDescriptor &ud,
                                     const ExecutionContext &) = 0;

    /**
     * Acknowledge the pending update `u`.
     *
     * If the update `u` doesn't target your module, ignore
     * it.
     */
    virtual void ack_update(IUpdatePtr u, const ExecutionContext &) = 0;

    /**
     * Cancel (not rollback) the pending update `u`.
     *
     * If the update `u` doesn't target your module, ignore
     * it.
     */
    virtual void cancel_update(IUpdatePtr u, const ExecutionContext &) = 0;
};

/**
 * This service provides various update management
 * utilities.
 *
 * It's goal is to unify update management between various
 * modules. Modules can register their UpdateBackend object
 * against this service in order to participates in update detection.
 *
 * @note This service is considered a "core service" and is therefore
 * always available.
 *
 * @warning The update service requires that database be enabled.
 */
class UpdateService
    : public ExtensibleSerializer<json, IUpdate, const SecurityContext &>
{
  public:
    std::vector<UpdateDescriptorPtr> check_update();

    /**
     * Create an Update object corresponding to the update descriptor
     * whose uuid is `update_descriptor_uuid`.
     *
     * @note Throw if the uuid doesn't reference anything anymore.
     *
     * @note We call create_update() for each registered backend with
     * a reference to the update descriptor. Backends can cast the
     * descriptor object to check whether the call is really for them or not.
     */
    IUpdatePtr create_update(const std::string &update_descriptor_uuid,
                             const ExecutionContext &ec);

    /**
     * Retrieve the list of pending updates.
     */
    std::vector<IUpdatePtr> pending_updates();

    void ack_update(IUpdatePtr update, const ExecutionContext &ec);

    void cancel_update(IUpdatePtr update, const ExecutionContext &ec);

    /**
     * Register a backend object.
     *
     * The backend is automatically deregistered when the `backend` object
     * is deleted.
     */
    void register_backend(UpdateBackendPtr backend);

  private:
    mutable std::mutex mutex_;

    using CheckUpdateT =
        boost::signals2::signal<std::vector<UpdateDescriptorPtr>(void),
                                VectorAppenderCombiner<UpdateDescriptorPtr>>;

    using CreateUpdateT =
        boost::signals2::signal<IUpdatePtr(const UpdateDescriptor &,
                                           const ExecutionContext &),
                                AtMostOneCombiner<IUpdatePtr>>;

    using AckUpdateT =
        boost::signals2::signal<void(IUpdatePtr, const ExecutionContext &)>;

    using CancelUpdateT =
        boost::signals2::signal<void(IUpdatePtr, const ExecutionContext &)>;

    CheckUpdateT check_update_sig_;
    CreateUpdateT create_update_sig_;
    AckUpdateT ack_update_sig_;
    CancelUpdateT cancel_update_sig_;

    std::map<std::string, UpdateDescriptorPtr> published_descriptors_;
};
}
}
