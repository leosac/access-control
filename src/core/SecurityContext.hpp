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

#include "core/audit/AuditFwd.hpp"
#include "core/auth/AuthFwd.hpp"
#include "core/credentials/CredentialFwd.hpp"
#include "hardware/HardwareFwd.hpp"
#include "tools/ToolsFwd.hpp"
#include "tools/db/db_fwd.hpp"

namespace Leosac
{
/**
 * A SecurityContext is used to query permission while doing
 * an operation.
 *
 * For example, JSON serializers could use a SecurityContext to determine what
 * to serialize.
 *
 * todo: fix doc
 */
class SecurityContext
{
  public:
    // Forward declare the union so we can write cast operator.
    union ActionParam;
    enum class Action
    {
        /**
         * A workaround permission that requires the user to be administrator
         */
        IS_ADMIN,
        /**
         * Requires that the user be at least manager.
         */
        IS_MANAGER,


        USER_CREATE,
        USER_READ,
        USER_READ_EMAIL,
        USER_UPDATE,
        USER_CHANGE_PASSWORD,
        USER_SEARCH,

        /**
         * Editing rank means being able to become administrator.
         */
        USER_UPDATE_RANK,
        /**
         * Can we enable/disable the user or change its
         * validity period ?
         */
        USER_MANAGE_VALIDITY,
        USER_DELETE,

        GROUP_CREATE,
        GROUP_READ,
        GROUP_UPDATE,
        GROUP_DELETE,
        GROUP_SEARCH,

        /**
         * Ability to list member of a group.
         * The ability to list membership gives USER_READ access against
         * the user whose membership is listed.
         */
        GROUP_LIST_MEMBERSHIP,
        GROUP_MEMBERSHIP_JOINED,
        GROUP_MEMBERSHIP_LEFT,
        MEMBERSHIP_READ,

        CREDENTIAL_READ,
        CREDENTIAL_UPDATE,
        CREDENTIAL_CREATE,
        CREDENTIAL_DELETE,

        SCHEDULE_READ,
        SCHEDULE_UPDATE,
        SCHEDULE_CREATE,
        SCHEDULE_DELETE,
        SCHEDULE_SEARCH,

        DOOR_READ,
        DOOR_UPDATE,
        DOOR_CREATE,
        DOOR_DELETE,
        DOOR_SEARCH,

        ACCESS_POINT_READ,
        ACCESS_POINT_CREATE,
        ACCESS_POINT_UPDATE,
        ACCESS_POINT_DELETE,
        ACCESS_POINT_SEARCH,

        ZONE_READ,
        ZONE_UPDATE,
        ZONE_CREATE,
        ZONE_DELETE,
        ZONE_SEARCH,

        /**
         * Retrieve SMTP configuration
         */
        SMTP_GETCONFIG,
        /**
         * Edit the SMTP configuration
         */
        SMTP_SETCONFIG,
        SMTP_SENDMAIL,

        LOG_READ,

        /**
         * Read the audit log
         */
        AUDIT_READ,
        /**
         * Read the audit log and access additional information, such
         * as the JSON "before" and "after" field.
         */
        AUDIT_READ_FULL,

        /**
         * Overview of users/doors access permission.
         */
        ACCESS_OVERVIEW,

        /**
         * Perform to restart the Leosac server.
         */
        RESTART_SERVER,

        /**
         * Permissions for hardware devices.
         * For now all hardware devices share the same permission set.
         * todo: Permission should probably redesigned in a more extensible way.
         */

        HARDWARE_READ,
        HARDWARE_UPDATE,
        HARDWARE_CREATE,
        HARDWARE_DELETE,
        HARDWARE_SEARCH,
    };

    struct GroupActionParam
    {
        Auth::GroupId group_id;

        operator ActionParam();
    };

    struct UserActionParam
    {
        Auth::UserId user_id;

        operator ActionParam();
    };

    struct CredentialActionParam
    {
        Cred::CredentialId credential_id;

        operator ActionParam();
    };

    struct MembershipActionParam
    {
        Auth::UserGroupMembershipId membership_id;
        Auth::GroupId group_id; // for create/delete
        Auth::UserId user_id;   // for create/delete
        Auth::GroupRank rank;   // for create

        operator ActionParam();
    };

    struct ScheduleActionParam
    {
        Tools::ScheduleId schedule_id;

        operator ActionParam();
    };

    struct DoorActionParam
    {
        Auth::DoorId door_id;

        operator ActionParam();
    };

    struct ZoneActionParam
    {
        Auth::ZoneId zone_id;

        operator ActionParam();
    };

    struct AccessPointActionParam
    {
        Auth::AccessPointId ap_id;

        operator ActionParam();
    };

    struct HardwareDeviceActionParam
    {
        Hardware::DeviceId device_id;

        operator ActionParam();
    };

    union ActionParam {
        GroupActionParam group;
        MembershipActionParam membership;
        UserActionParam user;
        CredentialActionParam cred;
        ScheduleActionParam sched;
        DoorActionParam door;
        AccessPointActionParam access_point;
        ZoneActionParam zone;
        HardwareDeviceActionParam device;
    };

    explicit SecurityContext(DBServicePtr dbsrv);
    virtual ~SecurityContext() = default;

    /**
     * Check for the permission to perform action `a` with parameters
     * `ap`.
     *
     * Returns true if the permission is granted, false otherwise.
     */
    virtual bool check_permission(Action a, const ActionParam &ap) const;

    /**
     * Check for the permission to perform a given action.
     * @return true if permission is granted, false otherwise.
     */
    bool check_permission(Action a) const;

    /**
     * Similar to check_permission(), but throws is the permission
     * is denied.
     */
    void enforce_permission(Action a, const ActionParam &ap) const;

    /**
     * Make sure that we have the permission to perform action `a`,
     * otherwise throws.
     */
    void enforce_permission(Action a) const;

  protected:
    DBServicePtr dbsrv_;

  private:
    /**
     * Reimplement this method to provide permission checking.
     */
    virtual bool check_permission_impl(Action a, const ActionParam &ap) const = 0;
};

/**
 * A security context for system operation.
 *
 * Basically, this security context always grants all permissions.
 */
class SystemSecurityContext : public SecurityContext
{
  public:
    explicit SystemSecurityContext(DBServicePtr dbsrv);

    static SecurityContext &instance();
    virtual bool check_permission_impl(Action a,
                                       const ActionParam &ap) const override;
};

/**
 * An ExecutionContext is passed around to service so they
 * have context about who is making the call and how.
 */
struct ExecutionContext
{
    explicit ExecutionContext(SecurityContext &sc);

    ExecutionContext(SecurityContext &sc, const Audit::IAuditEntryPtr &);


    /**
     * The SecurityContext of the caller.
     */
    SecurityContext &sec;

    /**
     * An optional audit object that would
     * act as parent for the audit trail.
     */
    Audit::IAuditEntryPtr audit;
};

struct SystemExecutionContext : public ExecutionContext
{
    SystemExecutionContext();
};
}
