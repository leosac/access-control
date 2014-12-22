/*
    Copyright (C) 2014-2015 Islog

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

#include "core/auth/Interfaces/IAuthenticationSource.hpp"
#include "FileAuthSourceMapper.hpp"
#include "core/auth/WiegandCard.hpp"
#include "tools/XmlPropertyTree.hpp"
#include "tools/log.hpp"
#include <iostream>
#include <exception/moduleexception.hpp>
#include <exception/configexception.hpp>
#include <boost/algorithm/string.hpp>
#include <core/auth/ProfileMerger.hpp>

using namespace Leosac::Module::Auth;
using namespace Leosac::Auth;

FileAuthSourceMapper::FileAuthSourceMapper(const std::string &auth_file) :
        config_file_(auth_file)
{
    try
    {
        authentication_data_ = Tools::propertyTreeFromXmlFile(auth_file);
        authentication_data_ = authentication_data_.get_child("root");
        build_permission();
    }
    catch (...)
    {
        std::throw_with_nested(ModuleException("AuthFile cannot load configuration"));
    }
}

void FileAuthSourceMapper::visit(WiegandCard *src)
{
    const boost::property_tree::ptree &mapping_tree = authentication_data_.get_child("user_mapping");

    for (const auto &mapping : mapping_tree)
    {
        const std::string &node_name = mapping.first;
        const boost::property_tree::ptree &node = mapping.second;

        if (node_name != "map")
            throw ConfigException(config_file_, "Invalid config file content");

        // does this entry map a wiegand card?
        auto opt_child = node.get_child_optional("WiegandCard");
        if (opt_child)
        {
            if (opt_child->data() == src->id())
            {
                // we found the card id
                std::string user_id = node.get<std::string>("user");

                // if user doesn't already exists this mean its profile is empty.
                // lets create the user on the fly, he'll still have no profile.
                if (!users_[user_id])
                    users_[user_id] = IUserPtr(new IUser(user_id));
                src->owner(users_[user_id]);
            }
        }
    }
}

void FileAuthSourceMapper::mapToUser(IAuthenticationSourcePtr auth_source)
{
    assert(auth_source.get());
    try
    {
        auth_source->accept(this);
    }
    catch (...)
    {
        std::throw_with_nested(ModuleException("AuthFile failed to map auth_source to user"));
    }
}

IAccessProfilePtr FileAuthSourceMapper::buildProfile(IAuthenticationSourcePtr auth_source)
{
    assert(auth_source);
    std::vector<GroupPtr> grps;
    std::vector<IAccessProfilePtr> profiles; // profiles that apply to the user.

    if (!auth_source->owner())
    {
        NOTICE("No owner for this auth source.");
        return nullptr;
    }

    grps = get_user_groups(auth_source->owner());
    for (auto & grp : grps)
            profiles.push_back(grp->profile());

    profiles.push_back(auth_source->owner()->profile());
    return merge_profiles(profiles);
}

void FileAuthSourceMapper::build_permission()
{
    const boost::property_tree::ptree &mapping_tree = authentication_data_.get_child("permissions");

    auto opt_group_mapping = authentication_data_.get_child_optional("group_mapping");
    if (opt_group_mapping)
        membership_group(*opt_group_mapping);

    for (const auto &permission_mapping : mapping_tree)
    {
        const std::string &node_name = permission_mapping.first;
        const boost::property_tree::ptree &node = permission_mapping.second;

        if (node_name != "map")
            throw ConfigException(config_file_, "Invalid config file content");

        auto opt_child_user = node.get_child_optional("user");
        auto opt_child_group = node.get_child_optional("group");
        if (opt_child_user)
        {
            permission_user(opt_child_user->data(), node);
        }
        else if (opt_child_group)
        {
            permission_group(opt_child_group->data(), node);
        }
    }
}

void FileAuthSourceMapper::build_schedule(Leosac::Auth::SimpleAccessProfilePtr profile,
        const boost::property_tree::ptree &schedule_cfg,
        bool is_default)
{
    assert(profile);
    std::string target_name;
    AuthTargetPtr target;

    if (!is_default)
    {
        // if target is "default" it doesn't apply for a specific one so we dont care about door name.
        target_name = schedule_cfg.get<std::string>("door");
        target = targets_[target_name];

        if (!target)
            target = targets_[target_name] = AuthTargetPtr(new AuthTarget(target_name));
    }

    // loop over every subnodes
    // we have to ignore the <door> name as it doesn't directly hold scheduling data
    for (const auto &schedule_info : schedule_cfg)
    {
        std::string day = schedule_info.first;
        std::vector<std::string> temp;

        if (day == "door")
            continue;
        std::string start = schedule_info.second.get<std::string>("start");
        std::string end = schedule_info.second.get<std::string>("end");

        boost::split(temp, start, boost::is_any_of(":"));
        if (temp.size() != 2)
            throw ModuleException("AuthFail schedule building error.");
        int start_hour = std::stoi(temp[0]);
        int start_min = std::stoi(temp[1]);

        temp.clear();
        boost::split(temp, end, boost::is_any_of(":"));
        if (temp.size() != 2)
            throw ModuleException("AuthFail schedule building error.");
        int end_hour = std::stoi(temp[0]);
        int end_min = std::stoi(temp[1]);

        profile->addAccessHour(target,
                week_day_to_int(day), start_hour, start_min, end_hour, end_min);
    }
}

int FileAuthSourceMapper::week_day_to_int(const std::string &day)
{
    if (day == "sunday")
        return 0;
    if (day == "monday")
        return 1;
    if (day == "tuesday")
        return 2;
    if (day == "wednesday")
        return 3;
    if (day == "thursday")
        return 4;
    if (day == "friday")
        return 5;
    if (day == "saturday")
        return 6;
    assert(0);
}

void FileAuthSourceMapper::permission_user(const std::string &user_name,
        const boost::property_tree::ptree &node)
{
    // we a are building permission for a user

    IUserPtr user = users_[user_name];
    if (user && user->profile())
        WARN("We already had data for this user. Will override");
    else if (!user)
    {
        user = IUserPtr(new IUser(user_name));
        users_[user_name] = user;
    }

    SimpleAccessProfilePtr profile(new SimpleAccessProfile());
    user->profile(profile);

    for (const auto &schedule : node)
    {
        if (schedule.first == "default_schedule")
            build_schedule(profile, schedule.second, true);
        if (schedule.first == "schedule")
            build_schedule(profile, schedule.second, false);
    }
}

void FileAuthSourceMapper::permission_group(const std::string &group_name,
        const boost::property_tree::ptree &node)
{
    GroupPtr group = groups_[group_name];

    if (group && group->profile())
        WARN("Already had some data for this group. Will override.");
    else if (!group)
    {
        group = groups_[group_name] = GroupPtr(new Group(group_name));
    }

    SimpleAccessProfilePtr profile(new SimpleAccessProfile());
    group->profile(profile);

    for (const auto &schedule : node)
    {
        if (schedule.first == "default_schedule")
            build_schedule(profile, schedule.second, true);
        if (schedule.first == "schedule")
            build_schedule(profile, schedule.second, false);
    }
}

void FileAuthSourceMapper::membership_group(const boost::property_tree::ptree &group_mapping)
{
    for (const auto &group_info : group_mapping)
    {
        const boost::property_tree::ptree &node = group_info.second;
        const std::string &group_name = node.get<std::string>("group");

        if (group_info.first != "map")
            throw ConfigException(config_file_, "Invalid config file content");

        GroupPtr grp = groups_[group_name] = GroupPtr(new Group(group_name));

        for (const auto &membership : node)
        {
            if (membership.first != "user")
                continue;
            std::string user_name = membership.second.data();
            IUserPtr user = users_[user_name];
            if (!user)
                user = users_[user_name] = IUserPtr(new IUser(user_name));
            grp->member_add(user);
        }
    }
}

std::vector<GroupPtr> FileAuthSourceMapper::groups() const
{
    std::vector<GroupPtr> ret;

    ret.reserve(groups_.size());
    for (const auto &map_entry : groups_)
    {
        ret.push_back(map_entry.second);
    }
    return ret;
}

std::vector<GroupPtr> FileAuthSourceMapper::get_user_groups(Leosac::Auth::IUserPtr u)
{
    assert(u);
    std::vector<GroupPtr> grps;
    auto lambda_cmp = [&](IUserPtr user) -> bool
            {
                return user->id() == u->id();
            };

    for (const auto &grp_map : groups_)
    {
        GroupPtr grp;
        std::tie(std::ignore, grp) = grp_map;

        if (std::find_if(grp->members().begin(),
                grp->members().end(), lambda_cmp) != grp->members().end())
        {
            grps.push_back(grp);
        }
    }
    return grps;
}

IAccessProfilePtr FileAuthSourceMapper::merge_profiles(const std::vector<IAccessProfilePtr> profiles)
{
    ProfileMerger merger;
    IAccessProfilePtr result(new SimpleAccessProfile());

    for (auto &profile : profiles)
    {
        if (profile)
            result = merger.merge(result, profile);
    }
    return result;
}
