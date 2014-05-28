/**
 * \file ixmlserializable.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief IXmlSerializable class declaration
 */

#ifndef IXMLSERIALIZABLE_HPP
#define IXMLSERIALIZABLE_HPP

#include <boost/property_tree/ptree.hpp>

class IXmlSerializable
{
public:
    virtual ~IXmlSerializable() {}
    virtual void    serialize(boost::property_tree::ptree& node) = 0;
    virtual void    deserialize(const boost::property_tree::ptree& node) = 0;
};

#endif // IXMLSERIALIZABLE_HPP
