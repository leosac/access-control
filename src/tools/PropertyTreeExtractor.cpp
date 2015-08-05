//
// Created by xaqq on 8/5/15.
//

#include "PropertyTreeExtractor.hpp"
#include <boost/property_tree/ptree.hpp>
#include "Colorize.hpp"
#include "log.hpp"

using namespace Leosac::Tools;

PropertyTreeExtractor::PropertyTreeExtractor(
        const boost::property_tree::ptree &tree, const std::string &config_for) :
        tree_(tree),
        text_(config_for)
{

}

template<typename T>
T PropertyTreeExtractor::get(const std::string &node_name)
{
    using namespace Colorize;
    try
    {
        return tree_.get<T>(node_name);
    }
    catch (const boost::property_tree::ptree_bad_path &)
    {
        Ex::Config e(text_, node_name, true);
        ERROR(e.what());
        std::throw_with_nested(e);
    }
    catch (const boost::property_tree::ptree_bad_data &)
    {
        Ex::Config e(text_, node_name, false);
        ERROR(e.what());
        std::throw_with_nested(e);
    }
}

template<typename T>
T PropertyTreeExtractor::get(const std::string &node_name,
                             const T &default_value)
{
    return tree_.get<T>(node_name, default_value);
}

// Instantiate the template for a few basic types.

template std::string PropertyTreeExtractor::get<std::string>(const std::string &);
template int         PropertyTreeExtractor::get<int>(const std::string &);
template uint64_t    PropertyTreeExtractor::get<uint64_t>(const std::string &);

// Ditto, but for accessor with default value.

template std::string PropertyTreeExtractor::get<std::string>(const std::string &, const std::string &);
template int         PropertyTreeExtractor::get<int>(const std::string &, const int &);
template uint64_t    PropertyTreeExtractor::get<uint64_t>(const std::string &, const uint64_t &);
