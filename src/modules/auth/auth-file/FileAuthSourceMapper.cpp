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

#include <boost/algorithm/string.hpp>

#include "core/auth/Interfaces/IAuthenticationSource.hpp"
#include "FileAuthSourceMapper.hpp"
#include "tools/XmlPropertyTree.hpp"
#include "tools/log.hpp"
#include "core/auth/ProfileMerger.hpp"
#include "exception/moduleexception.hpp"
#include "core/auth/Auth.hpp"

using namespace Leosac::Module::Auth;
using namespace Leosac::Auth;

FileAuthSourceMapper::FileAuthSourceMapper(const std::string &auth_file) :
        config_file_(auth_file)
{
    try
    {
        // Loading order:
        //          - Users
        //          - Groups
        //          - Credentials
        //          - Schedule, and schedule mapping

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

        const auto &credentials_tree = additional_config.get_child_optional("credentials");
        if (credentials_tree)
            load_credentials(*credentials_tree);

        const auto &schedules_tree = additional_config.get_child_optional("schedules");
        if (schedules_tree)
            load_schedules(*schedules_tree);

        const auto &schedule_mapping_tree = additional_config.get_child_optional("schedules_mapping");
        if (schedule_mapping_tree)
            map_schedules(*schedule_mapping_tree);

       DEBUG("Ready");
    }
    catch (std::exception &e)
    {
        ERROR("Exception: " << e.what());
        std::throw_with_nested(ModuleException("AuthFile cannot load configuration"));
    }
}

void FileAuthSourceMapper::visit(WiegandCard *src)
{
    auto it = wiegand_cards_.find(src->card_id());
    if (it != wiegand_cards_.end())
    {
        // By copying our instance of the credential to the
        // caller credential object, we store additional info we gathered
        // (like owner) and made them accessible to them.
        auto cred = it->second;
        // the auth source name is only set in the original credential object
        cred->name(src->name());
        *src = *cred;
    }
}

void FileAuthSourceMapper::visit(::Leosac::Auth::PINCode *src)
{
    auto it = pin_codes_.find(src->pin_code());
    if (it != pin_codes_.end())
    {
        // By copying our instance of the credential to the
        // caller credential object, we store additional info we gathered
        // (like owner) and made them accessible to them.
        auto cred = it->second;
        // the auth source name is only set in the original credential object
        cred->name(src->name());
        *src = *cred;
    }
}

void FileAuthSourceMapper::visit(::Leosac::Auth::WiegandCardPin *src)
{
    auto key = std::make_pair(src->card().card_id(), src->pin().pin_code());

    auto it = wiegand_cards_pins_.find(key);
    if (it != wiegand_cards_pins_.end())
    {
        // By copying our instance of the credential to the
        // caller credential object, we store additional info we gathered
        // (like owner) and made them accessible to them.
        auto cred = it->second;
        // the auth source name is only set in the original credential object
        cred->name(src->name());
        *src = *cred;
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

    if (!auth_source->validity().is_valid())
    {
        NOTICE("Credentials is invalid. It was disabled or out of its validity period.");
        return nullptr;
    }

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
    if (auth_source->profile())
    {
        profiles.push_back(auth_source->profile());
    }
    return merge_profiles(profiles);
}

void FileAuthSourceMapper::load_groups(const boost::property_tree::ptree &group_mapping)
{
    for (const auto &group_info : group_mapping)
    {
        const boost::property_tree::ptree &node = group_info.second;
        const std::string &group_name = node.get<std::string>("group");

        enforce_xml_node_name("map", group_info.first);

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

        enforce_xml_node_name("map", node_name);

        std::string user_id = node.get<std::string>("user");
        IUserPtr user       = users_[user_id];
        assert(user);

        IAuthenticationSourcePtr credential;

        // does this entry map a wiegand card?
        auto opt_child = node.get_child_optional("WiegandCard");
        if (opt_child)
        {
            std::string card_id = opt_child->get<std::string>("card_id");
            int bits            = opt_child->get<int>("bits");

            credential = std::make_shared<WiegandCard>(card_id, bits);
            wiegand_cards_[card_id] = std::static_pointer_cast<WiegandCard>(credential);
        }
        else if (opt_child = node.get_child_optional("PINCode"))
        {
            // or to a PIN code ?
            std::string pin     = opt_child->get<std::string>("pin");

            credential = std::make_shared<PINCode>(pin);
            pin_codes_[pin] = std::static_pointer_cast<PINCode>(credential);
        }
        else if (opt_child = node.get_child_optional("WiegandCardPin"))
        {
            std::string card_id = opt_child->get<std::string>("card_id");
            std::string pin     = opt_child->get<std::string>("pin");
            int bits            = opt_child->get<int>("bits");

            credential = std::make_shared<WiegandCardPin>(card_id, bits, pin);
            wiegand_cards_pins_[std::make_pair(card_id, pin)] =
                    std::static_pointer_cast<WiegandCardPin>(credential);
        }
        assert(opt_child);
        credential->validity(extract_credentials_validity(*opt_child));
        credential->owner(user);
        credential->id(opt_child->get<std::string>("id", ""));
        credential->profile(SimpleAccessProfilePtr(new SimpleAccessProfile()));
        add_cred_to_id_map(credential);
    }
}

void FileAuthSourceMapper::load_schedules(const boost::property_tree::ptree &schedules)
{
    bool ret = xml_schedules_.load(schedules);
    ASSERT_LOG(ret, "Failed to load schedules");
}

void FileAuthSourceMapper::map_schedules(const boost::property_tree::ptree &schedules_mapping)
{
    for (const auto & mapping_entry : schedules_mapping)
    {
        const std::string &node_name            = mapping_entry.first;
        const boost::property_tree::ptree &node = mapping_entry.second;

        enforce_xml_node_name("map", node_name);

        // we can map multiple schedule at once.
        std::list<std::string> schedule_names;
        std::list<std::string> user_names;
        std::list<std::string> group_names;
        std::list<std::string> credential_names;
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
            if (mapping_data.first == "credential")
                credential_names.push_back(mapping_data.second.data());
        }

        // now build object based on what we extracted.
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
            for (const auto &cred_id : credential_names)
            {
                DEBUG("CRED  = " << cred_id);
                IAuthenticationSourcePtr auth_src = find_cred_by_id(cred_id);
                assert(auth_src);
                SimpleAccessProfilePtr profile = std::dynamic_pointer_cast<SimpleAccessProfile>(auth_src->profile());
                add_schedule_to_profile(schedule_name, profile, target_door);
            }
        }
    }
}

void FileAuthSourceMapper::add_schedule_to_profile(const std::string &schedule_name,
        ::Leosac::Auth::SimpleAccessProfilePtr profile,
        const std::string &door_name)
{
    assert(xml_schedules_.schedules().count(schedule_name));
    const auto &sched = xml_schedules_.schedules().at(schedule_name);

    // auth target is hacky
    if (!door_name.empty())
        profile->addAccessSchedule(AuthTargetPtr(new AuthTarget(door_name)), sched);
    else
        profile->addAccessSchedule(nullptr, sched);
}

void FileAuthSourceMapper::load_users(const boost::property_tree::ptree &users)
{
    for (const auto &user : users)
    {
        const std::string &node_name            = user.first;
        const boost::property_tree::ptree &node = user.second;

        enforce_xml_node_name("user", node_name);

        std::string name        = node.get<std::string>("name");
        std::string firstname   = node.get<std::string>("firstname", "");
        std::string lastname    = node.get<std::string>("lastname", "");
        std::string email       = node.get<std::string>("email", "");

        IUserPtr uptr(new IUser(name));
        uptr->firstname(firstname);
        uptr->lastname(lastname);
        uptr->email(email);
        uptr->validity(extract_credentials_validity(node));

        // create an empty profile
        uptr->profile(SimpleAccessProfilePtr(new SimpleAccessProfile()));

        if (users_.count(name))
        {
            NOTICE("User " << name << " was already defined. Will overwrite.");
        }
        users_[name] = uptr;
    }
}

Leosac::Auth::CredentialValidity FileAuthSourceMapper::extract_credentials_validity(const boost::property_tree::ptree &node)
{
    CredentialValidity v;

    v.set_start_date(node.get<std::string>("validity_start", ""));
    v.set_end_date(node.get<std::string>("validity_end", ""));
    v.set_enabled(node.get<bool>("enabled", true));

    return v;
}

IAuthenticationSourcePtr FileAuthSourceMapper::find_cred_by_id(const std::string &id)
{
    auto &&itr = id_to_cred_.find(id);
    if (itr != id_to_cred_.end())
        return itr->second;
    return nullptr;
}

void FileAuthSourceMapper::add_cred_to_id_map(Leosac::Auth::IAuthenticationSourcePtr credential)
{
    if (!credential->id().empty())
    {
        if (id_to_cred_.count(credential->id()))
        {
            NOTICE("Credential with ID = " << credential->id() << " already exist and "
                    "will be overwritten.");
        }
        id_to_cred_[credential->id()] = credential;
    }
}

void FileAuthSourceMapper::enforce_xml_node_name(const std::string &expected,
                                                 const std::string &current)
{
    if (current != expected)
    {
        std::stringstream ss;
        ss << "Invalid configuration file content. Expected xml tag "
                << Colorize::green(expected) << " but has "
                << Colorize::green(current) << " instead.";
        throw ConfigException(config_file_, ss.str());
    }
}
