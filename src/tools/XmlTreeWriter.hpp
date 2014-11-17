#include <boost/property_tree/ptree_fwd.hpp>

#pragma once
namespace Leosac
{
    namespace Tools
    {
        /**
        * Write a property tree to an xml file.
        */
        void propertyTreeToXmlFile(const boost::property_tree::ptree &tree,
                const std::string &path);
    }
}
