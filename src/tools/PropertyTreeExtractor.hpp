//
// Created by xaqq on 8/5/15.
//

#ifndef LEOSAC_PROPERTYTREEEXTRACTOR_HPP
#define LEOSAC_PROPERTYTREEEXTRACTOR_HPP

#include <boost/property_tree/ptree_fwd.hpp>

namespace Leosac
{
namespace Tools
{
    /**
     * This class is a simpler helper to extract value from a property tree.
     *
     * It is convenient because it can catch boost exception and rethrow
     * application-level exception with more context for the end-user.
     *
     * The object is configured in its constructor and can then be used to extract
     * properties from the tree.
     */
    class PropertyTreeExtractor
    {
    public:
        /**
         * Construct the extract.
         *
         * @param tree A reference to the tree from which we'll be extracting property.
         * @param config_for A custom string that will be appended to the error message
         * in case the extraction fails.
         */
        PropertyTreeExtractor(const boost::property_tree::ptree &tree,
                              const std::string &config_for);

        /**
         * Extract a mandatory property from the tree.
         *
         * Throws a `ex::Configuration` exception if the key cannot be found,
         * or if it cannot be converted to the excepted type.
         */
        template<typename T>
        T get(const std::string &node_name);

        /**
         * Extract an optional property from the tree.
         *
         * It uses the default in case none is provided in the tree.
         */
        template<typename T>
        T get(const std::string &node_name, const T &default_value);

    private:
        const boost::property_tree::ptree &tree_;
        std::string text_;
    };
}
}

#endif //LEOSAC_PROPERTYTREEEXTRACTOR_HPP
