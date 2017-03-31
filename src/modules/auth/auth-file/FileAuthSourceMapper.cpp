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

#include "FileAuthSourceMapper.hpp"
#include "core/auth/Auth.hpp"
#include "core/auth/Door.hpp"
#include "core/auth/Group.hpp"
#include "core/auth/Interfaces/IAuthenticationSource.hpp"
#include "core/auth/ProfileMerger.hpp"
#include "core/auth/User.hpp"
#include "core/credentials/ICredential.hpp"
#include "core/credentials/PinCode.hpp"
#include "core/credentials/RFIDCard.hpp"
#include "core/credentials/RFIDCardPin.hpp"
#include "exception/moduleexception.hpp"
#include "tools/AssertCast.hpp"
#include "tools/Schedule.hpp"
#include "tools/XmlPropertyTree.hpp"
#include "tools/log.hpp"
#include <boost/algorithm/string.hpp>
#include <tools/enforce.hpp>

using namespace Leosac::Module::Auth;
using namespace Leosac::Auth;

FileAuthSourceMapper::FileAuthSourceMapper(const std::string &auth_file)
    : config_file_(auth_file)
    , xmlnne_(config_file_)
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
        additional_config        = additional_config.get_child("root");
        DEBUG("Tree loaded");

        const auto &users_tree = additional_config.get_child_optional("users");
        if (users_tree)
            load_users(*users_tree);

        const auto &groups_tree =
            additional_config.get_child_optional("group_mapping");
        if (groups_tree)
            load_groups(*groups_tree);

        const auto &credentials_tree =
            additional_config.get_child_optional("credentials");
        if (credentials_tree)
            load_credentials(*credentials_tree);

        const auto &schedules_tree =
            additional_config.get_child_optional("schedules");
        if (schedules_tree)
            load_schedules(*schedules_tree);

        const auto &schedule_mapping_tree =
            additional_config.get_child_optional("schedules_mapping");
        if (schedule_mapping_tree)
            map_schedules(*schedule_mapping_tree);

        DEBUG("Ready");
    }
    catch (std::exception &e)
    {
        ERROR("Exception: " << e.what());
        std::throw_with_nested(
            ModuleException("AuthFile cannot load configuration"));
    }
}

void FileAuthSourceMapper::visit(::Leosac::Cred::RFIDCard &src)
{
    auto it = rfid_cards_.find(src.card_id());
    DEBUG("VISITING CARD" << src.card_id() << "");
    if (it != rfid_cards_.end())
    {
        auto cred = it->second;

        // By copying our instance of the credential to the
        // caller credential object, we store additional info we gathered
        // (like owner, id, or validity) and made them accessible to them.
        src = *cred;
    }
}

void FileAuthSourceMapper::visit(::Leosac::Cred::PinCode &src)
{
    auto it = pin_codes_.find(src.pin_code());
    if (it != pin_codes_.end())
    {
        auto cred = it->second;

        // By copying our instance of the credential to the
        // caller credential object, we store additional info we gathered
        // (like owner, id, or validity) and made them accessible to them.
        src = *cred;
    }
}

void FileAuthSourceMapper::visit(::Leosac::Cred::RFIDCardPin &src)
{
    auto key = std::make_pair(src.card().card_id(), src.pin().pin_code());

    auto it = rfid_cards_pin.find(key);
    if (it != rfid_cards_pin.end())
    {
        auto cred = it->second;

        // By copying our instance of the credential to the
        // caller credential object, we store additional info we gathered
        // (like owner) and made them accessible to them.
        src = *cred;
    }
}

void FileAuthSourceMapper::mapToUser(Cred::ICredentialPtr cred)
{
    ASSERT_LOG(cred, "Credential is null.");
    try
    {
        cred->accept(*this);
    }
    catch (...)
    {
        std::throw_with_nested(
            ModuleException("AuthFile failed to map auth_source to user"));
    }
}

void FileAuthSourceMapper::load_groups(
    const boost::property_tree::ptree &group_mapping)
{
    GroupId group_id =
        1; // Similar to user, we need ID to identify group in mapping.
    for (const auto &group_info : group_mapping)
    {
        const boost::property_tree::ptree &node = group_info.second;
        const std::string &group_name           = node.get<std::string>("group");

        xmlnne_("map", group_info.first);

        GroupPtr grp = groups_[group_name] = GroupPtr(new Group(group_name));
        grp->id(group_id++);
        grp->profile(SimpleAccessProfilePtr(new SimpleAccessProfile()));

        for (const auto &membership : node)
        {
            if (membership.first != "user")
                continue;
            std::string user_name = membership.second.data();
            UserPtr user          = users_[user_name];
            if (!user)
            {
                ERROR("Unknown user " << user_name);
                throw ConfigException(config_file_, "Unknown user " + user_name);
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

std::vector<GroupPtr> FileAuthSourceMapper::get_user_groups(Leosac::Auth::UserPtr u)
{
    assert(u);
    std::vector<GroupPtr> grps;
    auto lambda_cmp = [&](UserPtr user) -> bool {
        return user->username() == u->username();
    };

    for (const auto &grp_map : groups_)
    {
        GroupPtr grp;
        std::tie(std::ignore, grp) = grp_map;

        if (std::find_if(grp->members().begin(), grp->members().end(), lambda_cmp) !=
            grp->members().end())
        {
            grps.push_back(grp);
        }
    }
    return grps;
}

IAccessProfilePtr
FileAuthSourceMapper::merge_profiles(const std::vector<IAccessProfilePtr> profiles)
{
    ProfileMerger merger;
    IAccessProfilePtr result(new SimpleAccessProfile());

    for (auto &profile : profiles)
    {
        if (profile)
            result = merger.merge(result, profile);
    }
    if (result->schedule_count())
        return result;
    return nullptr;
}

void FileAuthSourceMapper::load_credentials(
    const boost::property_tree::ptree &credentials)
{
    Cred::CredentialId cred_id = 1;
    for (const auto &mapping : credentials)
    {
        const std::string &node_name            = mapping.first;
        const boost::property_tree::ptree &node = mapping.second;

        xmlnne_("map", node_name);

        std::string user_id = node.get<std::string>("user");
        UserPtr user        = users_[user_id];
        if (!user)
            throw ConfigException(
                config_file_, "Credentials defined for undefined user " + user_id);
        assert(user);

        Cred::ICredentialPtr credential;

        // does this entry map a wiegand card?
        auto opt_child = node.get_child_optional("WiegandCard");
        if (opt_child)
        {
            std::string card_id = opt_child->get<std::string>("card_id");
            int bits            = opt_child->get<int>("bits");

            Cred::RFIDCardPtr c = std::make_shared<Cred::RFIDCard>();
            c->card_id(card_id);
            c->nb_bits(bits);
            rfid_cards_[card_id] = c;
            credential           = c;
        }
        else if (opt_child = node.get_child_optional("PINCode"))
        {
            // or to a PIN code ?
            std::string pin = opt_child->get<std::string>("pin");

            Cred::PinCodePtr p = std::make_shared<Cred::PinCode>();
            p->pin_code(pin);
            pin_codes_[pin] = p;
            credential      = p;
        }
        else if (opt_child = node.get_child_optional("WiegandCardPin"))
        {
            std::string card_id = opt_child->get<std::string>("card_id");
            std::string pin     = opt_child->get<std::string>("pin");
            int bits            = opt_child->get<int>("bits");

            auto c = std::make_shared<Cred::RFIDCard>();
            c->id(cred_id++);
            c->card_id(card_id);
            c->nb_bits(bits);

            auto p = std::make_shared<Cred::PinCode>();
            p->id(cred_id++);
            p->pin_code(pin);
            credential = std::make_shared<Cred::RFIDCardPin>(c, p);
            rfid_cards_pin[std::make_pair(card_id, pin)] =
                assert_cast<Cred::RFIDCardPinPtr>(credential);
        }
        assert(opt_child);
        credential->id(cred_id++);
        credential->validity(extract_credentials_validity(*opt_child));
        credential->owner(user);

        // Alias in place of id, so that it can be a string (making it easier to
        // configure from the a XML file)
        credential->alias(opt_child->get<std::string>("id", ""));
        add_cred_to_id_map(credential);
    }
}

void FileAuthSourceMapper::load_schedules(
    const boost::property_tree::ptree &schedules)
{
    bool ret = xml_schedules_.load(schedules);
    ASSERT_LOG(ret, "Failed to load schedules");
}

void FileAuthSourceMapper::map_schedules(
    const boost::property_tree::ptree &schedules_mapping)
{
    for (const auto &mapping_entry : schedules_mapping)
    {
        const std::string &node_name            = mapping_entry.first;
        const boost::property_tree::ptree &node = mapping_entry.second;

        xmlnne_("map", node_name);

        // we can map multiple schedule at once.
        std::list<std::string> schedule_names;
        std::list<std::string> user_names;
        std::list<std::string> group_names;
        std::list<std::string> credential_names;
        std::string target_door;
        target_door = node.get<std::string>("door", "");
        auto door(std::make_shared<Leosac::Auth::Door>());
        door->alias(target_door);
        doors_.push_back(door);

        // lets loop over all the info we have
        for (const auto &mapping_data : node)
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
        for (const auto &schedule_name : schedule_names)
        {
            // Each schedule can be mapped once per ScheduleMapping, but can be
            // referenced by multiple schedule mapping. What we do here is for each
            // schedule in the mapping entry, we create a ScheduleMapping object.
            Tools::ScheduleMappingPtr sm(std::make_shared<Tools::ScheduleMapping>());
            xml_schedules_.schedules().at(schedule_name)->add_mapping(sm);

            if (!door->alias().empty())
                sm->add_door(door);

            for (const auto &user_name : user_names)
            {
                UserPtr user = users_[user_name];
                sm->add_user(user);
            }
            // now for groups
            for (const auto &group_name : group_names)
            {
                GroupPtr grp = groups_[group_name];
                sm->add_group(grp);
            }
            for (const auto &cred_id : credential_names)
            {
                DEBUG("CRED  = " << cred_id);
                Cred::ICredentialPtr cred = find_cred_by_alias(cred_id);
                sm->add_credential(assert_cast<Cred::CredentialPtr>(cred));
            }

            mappings_.push_back(sm);
        }
    }
}

void FileAuthSourceMapper::load_users(const boost::property_tree::ptree &users)
{
    // We use the user id internally to uniquely identify user
    // through ScheduleMapping.
    UserId user_id = 1;
    for (const auto &user : users)
    {
        const std::string &node_name            = user.first;
        const boost::property_tree::ptree &node = user.second;

        xmlnne_("user", node_name);

        std::string username  = node.get<std::string>("name");
        std::string firstname = node.get<std::string>("firstname", "");
        std::string lastname  = node.get<std::string>("lastname", "");
        std::string email     = node.get<std::string>("email", "");

        if (username == "UNKNOWN_USER") // reserved username
            throw ConfigException(config_file_,
                                  "'UNKNOWN_USER' is a reserved name. Do not use.");

        UserPtr uptr(std::make_unique<User>(user_id++));
        uptr->username(username);
        uptr->firstname(firstname);
        uptr->lastname(lastname);
        uptr->email(email);
        uptr->validity(extract_credentials_validity(node));

        // create an empty profile
        uptr->profile(SimpleAccessProfilePtr(new SimpleAccessProfile()));

        if (users_.count(username))
        {
            NOTICE("User " << username << " was already defined. Will overwrite.");
        }
        users_[username] = uptr;
    }
}

Leosac::Auth::ValidityInfo FileAuthSourceMapper::extract_credentials_validity(
    const boost::property_tree::ptree &node)
{
    ValidityInfo v;

    v.set_start_date(node.get<std::string>("validity_start", ""));
    v.set_end_date(node.get<std::string>("validity_end", ""));
    v.set_enabled(node.get<bool>("enabled", true));

    return v;
}

Leosac::Cred::ICredentialPtr
FileAuthSourceMapper::find_cred_by_alias(const std::string &alias)
{
    auto &&itr = id_to_cred_.find(alias);
    if (itr != id_to_cred_.end())
        return itr->second;
    return nullptr;
}

void FileAuthSourceMapper::add_cred_to_id_map(
    Leosac::Cred::ICredentialPtr credential)
{
    if (!credential->alias().empty())
    {
        if (id_to_cred_.count(credential->alias()))
        {
            NOTICE("Credential with ID = " << credential->alias()
                                           << " already exist and "
                                              "will be overwritten.");
        }
        id_to_cred_[credential->alias()] = credential;
    }
}

Leosac::Auth::IAccessProfilePtr
FileAuthSourceMapper::buildProfile(Leosac::Cred::ICredentialPtr cred)
{
    assert(cred);
    std::vector<GroupPtr> grps;
    std::vector<IAccessProfilePtr> profiles; // profiles that apply to the user.

    // Sanity check
    if (cred->owner())
        ASSERT_LOG(cred->owner().get_eager(), "Sanity check failed.");

    auto cred_owner = cred->owner().get_eager();
    if (!cred->validity().is_valid())
    {
        NOTICE("Credentials is invalid. It was disabled or out of its validity "
               "period.");
        return nullptr;
    }

    if (cred_owner && !cred_owner->is_valid())
    {
        NOTICE("The user (" << cred_owner->username()
                            << ") is disabled or out of its validity period.");
        return nullptr;
    }

    // First, create the profile for the user, if any
    if (cred_owner)
    {
        profiles.push_back(build_user_profile(cred_owner));

        // Profile for user's groups.
        grps = get_user_groups(cred_owner);
        for (auto &grp : grps)
            profiles.push_back(build_group_profile(grp));
    }

    profiles.push_back(build_cred_profile(cred));

    return merge_profiles(profiles);
}

SimpleAccessProfilePtr FileAuthSourceMapper::build_user_profile(UserPtr u)
{
    ASSERT_LOG(u, "User is null");

    auto profile(std::make_shared<SimpleAccessProfile>());
    for (const auto &mapping : mappings_)
    {
        if (mapping->has_user(u->id()))
        {
            auto schedule = mapping->schedule().get_eager().lock();
            if (!schedule)
            {
                WARN("Schedule is null in FileAuthSourceMapper::build_user_profile");
                continue;
            }
            if (mapping->doors().size())
            {
                // If we have doors, add the schedule against each door (AuthTarget)
                for (auto lazy_weak_door : mapping->doors())
                {
                    auto door_ptr = LEOSAC_ENFORCE(lazy_weak_door.get_eager().lock(),
                                                   "Cannot get Door from mapping");
                    profile->addAccessSchedule(
                        std::make_shared<AuthTarget>(door_ptr->alias()),
                        std::static_pointer_cast<const Tools::ISchedule>(schedule));
                }
            }
            else
            {
                // No specific door, add with nullptr as a target.
                profile->addAccessSchedule(
                    nullptr,
                    std::static_pointer_cast<const Tools::ISchedule>(schedule));
            }
        }
    }
    return profile;
}

SimpleAccessProfilePtr FileAuthSourceMapper::build_group_profile(GroupPtr g)
{
    ASSERT_LOG(g, "Group is null");

    auto profile(std::make_shared<SimpleAccessProfile>());
    for (const auto &mapping : mappings_)
    {
        DEBUG("MAPPING HAS " << mapping->groups().size() << " GROUPS");
        if (mapping->has_group(g->id()))
        {
            auto schedule = mapping->schedule().get_eager().lock();
            if (!schedule)
            {
                WARN(
                    "Schedule is null in FileAuthSourceMapper::build_group_profile");
                continue;
            }
            if (mapping->doors().size())
            {
                // If we have doors, add the schedule against each door (AuthTarget)
                for (auto lazy_weak_door : mapping->doors())
                {
                    auto door_ptr = LEOSAC_ENFORCE(lazy_weak_door.get_eager().lock(),
                                                   "Cannot get Door from mapping");
                    profile->addAccessSchedule(
                        std::make_shared<AuthTarget>(door_ptr->alias()),
                        std::static_pointer_cast<const Tools::ISchedule>(schedule));
                }
            }
            else
            {
                // No specific door, add with nullptr as a target.
                profile->addAccessSchedule(
                    nullptr,
                    std::static_pointer_cast<const Tools::ISchedule>(schedule));
            }
        }
    }
    return profile;
}

Leosac::Auth::SimpleAccessProfilePtr
FileAuthSourceMapper::build_cred_profile(Leosac::Cred::ICredentialPtr c)
{
    ASSERT_LOG(c, "Credential is null");

    auto profile(std::make_shared<SimpleAccessProfile>());
    for (const auto &mapping : mappings_)
    {
        if (mapping->has_cred(c->id()))
        {
            auto schedule = mapping->schedule().get_eager().lock();
            if (!schedule)
            {
                WARN("Schedule is null in FileAuthSourceMapper::build_cred_profile");
                continue;
            }
            if (mapping->doors().size())
            {
                // If we have doors, add the schedule against each door (AuthTarget)
                for (auto lazy_weak_door : mapping->doors())
                {
                    auto door_ptr = LEOSAC_ENFORCE(lazy_weak_door.get_eager().lock(),
                                                   "Cannot get Door from mapping");
                    profile->addAccessSchedule(
                        std::make_shared<AuthTarget>(door_ptr->alias()),
                        std::static_pointer_cast<const Tools::ISchedule>(schedule));
                }
            }
            else
            {
                // No specific door, add with nullptr as a target.
                profile->addAccessSchedule(
                    nullptr,
                    std::static_pointer_cast<const Tools::ISchedule>(schedule));
            }
        }
    }
    if (profile->schedule_count() == 0)
        return nullptr;
    return profile;
}
