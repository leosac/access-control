#include "core/auth/Interfaces/IAuthenticationSource.hpp"
#include "FileAuthSourceMapper.hpp"
#include "core/auth/WiegandCard.hpp"
#include "tools/XmlTreeBuilder.hpp"
#include <iostream>
#include <exception/moduleexception.hpp>
#include <exception/configexception.hpp>


using namespace Leosac::Module::Auth;
using namespace Leosac::Auth;

FileAuthSourceMapper::FileAuthSourceMapper(const std::string &auth_file) :
        config_file_(auth_file)
{
    try
    {
        authentication_data_ = Tools::propertyTreeFromXmlFile(auth_file);
        authentication_data_ = authentication_data_.get_child("root");
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
                src->owner(IUserPtr(new IUser(user_id)));
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
    return std::shared_ptr<IAccessProfile>();
}
