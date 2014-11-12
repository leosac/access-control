#include "core/auth/Interfaces/IAuthenticationSource.hpp"
#include "FileAuthSourceMapper.hpp"
#include "core/auth/WiegandCard.hpp"
#include "tools/XmlTreeBuilder.hpp"
#include "tools/log.hpp"
#include <iostream>
#include <exception/moduleexception.hpp>
#include <exception/configexception.hpp>
#include <boost/algorithm/string.hpp>

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
    if (auth_source->owner())
    {
        return auth_source->owner()->profile();
    }
    return nullptr;
}

void FileAuthSourceMapper::build_permission()
{
    const boost::property_tree::ptree &mapping_tree = authentication_data_.get_child("permissions");

    for (const auto &permission_mapping : mapping_tree)
    {
        const std::string &node_name = permission_mapping.first;
        const boost::property_tree::ptree &node = permission_mapping.second;

        if (node_name != "map")
            throw ConfigException(config_file_, "Invalid config file content");

        auto opt_child_user = node.get_child_optional("user");
        if (opt_child_user)
        {
            // we a are mapping a user
            std::string user_name = opt_child_user->data();
            IUserPtr user = users_[user_name];
            if (!user)
            {
                user = IUserPtr(new IUser(user_name));
                users_[user_name] = user;
            }
            else
                WARN("We already had data for this user");

            SimpleAccessProfilePtr profile(new SimpleAccessProfile());
            user->profile(profile);

            for (const auto &schedule : node)
            {
                if (schedule.first != "schedule")
                    continue;
                build_schedule(profile, schedule.second);
            }
        }
    }
}

void FileAuthSourceMapper::build_schedule(Leosac::Auth::SimpleAccessProfilePtr profile,
        const boost::property_tree::ptree &schedule_cfg)
{
    assert(profile);
    std::string target_name = schedule_cfg.get<std::string>("door");
    AuthTargetPtr target = targets_[target_name];

    if (!target)
        target = targets_[target_name] = AuthTargetPtr(new AuthTarget(target_name));

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
        if (!temp.size() == 2)
            throw ModuleException("AuthFail schedule building error.");
        int start_hour = std::stoi(temp[0]);
        int start_min = std::stoi(temp[1]);

        temp.clear();
        boost::split(temp, end, boost::is_any_of(":"));
        if (!temp.size() == 2)
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
