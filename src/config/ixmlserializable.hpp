/**
 * \file ixmlserializable.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief IXmlSerializable class declaration
 */

#ifndef IXMLSERIALIZABLE_HPP
#define IXMLSERIALIZABLE_HPP

#include <boost/property_tree/ptree.hpp>

using boost::property_tree::ptree;

class IXmlSerializable
{
public:
    virtual ~IXmlSerializable() {}
    virtual void    serialize(ptree& node) = 0;
    virtual void    deserialize(const ptree& node) = 0;
};

#endif // IXMLSERIALIZABLE_HPP
