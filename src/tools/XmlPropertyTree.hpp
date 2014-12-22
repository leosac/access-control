#include <boost/property_tree/ptree_fwd.hpp>

#pragma once

namespace Leosac
{
    namespace Tools
    {
        /**
        * Build a property tree from a xml file.
        * @param path Path to the xml file.
        */
        boost::property_tree::ptree propertyTreeFromXmlFile(const std::string &path);

        /**
        * Write a property tree to an xml file.
        */
        void propertyTreeToXmlFile(const boost::property_tree::ptree &tree,
                const std::string &path);
    }
}
