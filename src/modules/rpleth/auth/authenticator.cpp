/**
 * \file authenticator.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Authenticator class implementation
 */

#include "authenticator.hpp"

void Authenticator::serialize(boost::property_tree::ptree& node)
{
    boost::property_tree::ptree& child = node.add("auth", std::string());

    child.put("user", "test"); // TODO
}

void Authenticator::deserialize(boost::property_tree::ptree& node)
{
    for (auto& v : node)
    {
        if (v.first == "auth")
        {
            // TODO
        }
    }
}
