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
#include "tools/XmlPropertyTree.hpp"
#include "tools/log.hpp"
#include <boost/algorithm/string.hpp>
#include <core/auth/ProfileMerger.hpp>
#include "core/auth/Auth.hpp"

using namespace Leosac::Module::Auth;
using namespace Leosac::Auth;

FileAuthSourceMapper::FileAuthSourceMapper(const std::string &auth_file) :
        config_file_(auth_file)
{
    try
    {
        // load config file.
        // load/build group and users.
        // load schedule.
        // map schedule to group and user.
        // load credentials.
        DEBUG("Will load tree");
        auto &&additional_config = Tools::propertyTreeFromXmlFile(auth_file);
        additional_config = additional_config.get_child("root");
        DEBUG("Tree loaded");

        const auto &users_tree = additional_config.get_child_optional("users");
        if (users_tree)
            load_users(*users_tree);

        const auto &groups_tree = additional_config.get_child_optional("group_mapping");
        if (groups_tree)
            load_groups(*groups_tree);

        const auto &schedules_tree = additional_config.get_child_optional("schedules");
        if (schedules_tree)
            load_schedules(*schedules_tree);

        const auto &schedule_mapping_tree = additional_config.get_child_optional("schedules_mapping");
        if (schedule_mapping_tree)
            map_schedules(*schedule_mapping_tree);

        const auto &credentials_tree = additional_config.get_child_optional("credentials");
        if (credentials_tree)
            load_credentials(*credentials_tree);
    }
    catch (std::exception &e)
    {
        ERROR("Exception: " << e.what());
        std::throw_with_nested(ModuleException("AuthFile cannot load configuration"));
    }
}

void FileAuthSourceMapper::visit(WiegandCard *src)
{
    if (wiegand_card_user_map_.count(src->id()))
    {
        std::string user_id = wiegand_card_user_map_[src->id()];
        assert(!user_id.empty());
        assert(users_.count(user_id));
        src->owner(users_[user_id]);
    }
}

void FileAuthSourceMapper::visit(::Leosac::Auth::PINCode *src)
{
    if (pin_code_user_map_.count(src->pin_code()))
    {
        std::string user_id = pin_code_user_map_[src->pin_code()];
        assert(!user_id.empty());
        assert(users_.count(user_id));
        src->owner(users_[user_id]);
    }
}

void FileAuthSourceMapper::visit(::Leosac::Auth::WiegandCardPin *src)
{
    auto key = std::make_pair(src->card().id(), src->pin().pin_code());

    if (wiegand_card_pin_code_user_map_.count(key))
    {
        std::string user_id = wiegand_card_pin_code_user_map_[key];
        assert(!user_id.empty());
        assert(users_.count(user_id));
        src->owner(users_[user_id]);
    }
}

void FileAuthSourceMapper::mapToUser(IAuthenticationSourcePtr auth_source)
{
    assert(auth_source);
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

    if (!auth_source->owner()->is_valid())
    {
        NOTICE("The user (" << auth_source->owner()->id() << ") is disabled or out of its validity period.");
        return nullptr;
    }

    grps = get_user_groups(auth_source->owner());
    for (auto & grp : grps)
        profiles.push_back(grp->profile());

    profiles.push_back(auth_source->owner()->profile());
    return merge_profiles(profiles);
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

void FileAuthSourceMapper::load_groups(const boost::property_tree::ptree &group_mapping)
{
    for (const auto &group_info : group_mapping)
    {
        const boost::property_tree::ptree &node = group_info.second;
        const std::string &group_name = node.get<std::string>("group");

        if (group_info.first != "map")
            throw ConfigException(config_file_, "Invalid config file content");

        GroupPtr grp = groups_[group_name] = GroupPtr(new Group(group_name));
        grp->profile(SimpleAccessProfilePtr(new SimpleAccessProfile()));

        for (const auto &membership : node)
        {
            if (membership.first != "user")
                continue;
            std::string user_name = membership.second.data();
            IUserPtr user = users_[user_name];
            if (!user)
            {
                ERROR("Unkown user " << user_name);
                throw ConfigException(config_file_, "Unkown user " + user_name);
            }
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

void FileAuthSourceMapper::load_credentials(const boost::property_tree::ptree &credentials)
{
    for (const auto &mapping : credentials)
    {
        const std::string &node_name = mapping.first;
        const boost::property_tree::ptree &node = mapping.second;

        if (node_name != "map")
            throw ConfigException(config_file_, "Invalid config file content");


        std::string user_id = node.get<std::string>("user");

        // does this entry map a wiegand card?
        auto opt_child = node.get_child_optional("WiegandCard");
        if (opt_child)
        {
            std::string card_id = opt_child->data();
            wiegand_card_user_map_[card_id] = user_id;
        }
        else if (opt_child = node.get_child_optional("PINCode"))
        {
            // or to a PIN code ?
            std::string pin = opt_child->data();
            pin_code_user_map_[pin] = user_id;
        }
        else if (opt_child = node.get_child_optional("WiegandCardPin"))
        {
            std::string card_id = opt_child->get<std::string>("card");
            std::string pin = opt_child->get<std::string>("pin");
            wiegand_card_pin_code_user_map_[std::make_pair(card_id, pin)] = user_id;
        }
    }
}

void FileAuthSourceMapper::load_schedules(const boost::property_tree::ptree &schedules)
{
    for (const auto & sched : schedules)
    {
        const std::string &node_name            = sched.first;
        const boost::property_tree::ptree &node = sched.second;

        if (node_name != "schedule")
            throw ConfigException(config_file_, "Invalid config file content. Expected "
                    "a node named 'schedule', found " + node_name + " instead");

        std::string schedule_name   = node.get<std::string>("name");
        Schedule time_frame_list;

        // loop on all properties of the schedule.
        // those will be weekday and the <name> tag too.
        for (const auto & sched_data : node)
        {
            if (sched_data.first == "name") // we already got the name.
                continue;
            std::string start           = sched_data.second.get<std::string>("start");
            std::string end             = sched_data.second.get<std::string>("end");
            std::vector<std::string> temp;

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

            Tools::SingleTimeFrame tf(week_day_to_int(sched_data.first),
            start_hour, start_min,
                    end_hour, end_min);
            time_frame_list.push_back(tf);
        }
        unmapped_schedules_[schedule_name] = time_frame_list;
    }
}

void FileAuthSourceMapper::map_schedules(const boost::property_tree::ptree &schedules_mapping)
{
    for (const auto & mapping_entry : schedules_mapping)
    {
        const std::string &node_name            = mapping_entry.first;
        const boost::property_tree::ptree &node = mapping_entry.second;

        if (node_name != "map")
            throw ConfigException(config_file_, "Invalid config file content");

        // we can map multiple schedule at once.
        std::list<std::string> schedule_names;
        std::list<std::string> user_names;
        std::list<std::string> group_names;
        std::string target_door;
        target_door = node.get<std::string>("door", "");

        // lets loop over all the info we have
        for (const auto & mapping_data : node)
        {
            if (mapping_data.first == "door")
                continue;
            if (mapping_data.first == "schedule")
                schedule_names.push_back(mapping_data.second.data());
            if (mapping_data.first == "user")
                user_names.push_back(mapping_data.second.data());
            if (mapping_data.first == "group")
                group_names.push_back(mapping_data.second.data());
        }

        for (const auto & schedule_name : schedule_names)
        {
            for (const auto &user_name : user_names)
            {
                IUserPtr user = users_[user_name];
                assert(user);
                assert(user->profile());
                SimpleAccessProfilePtr profile = std::dynamic_pointer_cast<SimpleAccessProfile>(user->profile());
                assert(profile);
                add_schedule_to_profile(schedule_name, profile, target_door);
               }
            // now for groups
            for (const auto &group_name : group_names)
            {
                GroupPtr grp = groups_[group_name];
                assert(grp);
                assert(grp->profile());
                SimpleAccessProfilePtr profile = std::dynamic_pointer_cast<SimpleAccessProfile>(grp->profile());
                assert(profile);
                add_schedule_to_profile(schedule_name, profile, target_door);
            }
        }
    }
}

void FileAuthSourceMapper::add_schedule_to_profile(const std::string &schedule_name,
        ::Leosac::Auth::SimpleAccessProfilePtr profile,
        const std::string &door_name)
{
    assert(unmapped_schedules_.count(schedule_name));
    for (const auto & sched_part : unmapped_schedules_[schedule_name])
    {
        // auth target is hacky
        if (!door_name.empty())
            profile->addAccessTimeFrame(AuthTargetPtr(new AuthTarget(door_name)), sched_part);
        else
            profile->addAccessTimeFrame(nullptr, sched_part);
    }
}

void FileAuthSourceMapper::load_users(const boost::property_tree::ptree &users)
{
    for (const auto &user : users)
    {
        const std::string &node_name            = user.first;
        const boost::property_tree::ptree &node = user.second;

        if (node_name != "user")
            throw ConfigException(config_file_, "Invalid config file content");
        std::string name        = node.get<std::string>("name");
        std::string firstname   = node.get<std::string>("firstname", "");
        std::string lastname    = node.get<std::string>("lastname", "");
        std::string email       = node.get<std::string>("email", "");
        CredentialValidity v;

        v.set_start_date(node.get<std::string>("validity_start", ""));
        v.set_end_date(node.get<std::string>("validity_end", ""));
        v.set_enabled(node.get<bool>("enabled", true));

        IUserPtr uptr(new IUser(name));
        uptr->firstname(firstname);
        uptr->lastname(lastname);
        uptr->email(email);
        uptr->validity(v);

        // create an empty profile
        uptr->profile(SimpleAccessProfilePtr(new SimpleAccessProfile()));

        if (users_.count(name))
        {
            NOTICE("User " << name << " was already defined. Will overwrite.");
        }
        users_[name] = uptr;
    }
}
