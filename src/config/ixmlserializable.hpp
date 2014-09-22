/**
 * \file ixmlserializable.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief IXmlSerializable class declaration
 */

#ifndef IXMLSERIALIZABLE_HPP
#define IXMLSERIALIZABLE_HPP

#include <boost/property_tree/ptree.hpp>

using boost::property_tree::ptree;

/**
* Interface that defines a Serializable object.
*
* Objects implementing this interface must be able to:
*      1. Serialize themselves into a property tree.
*      2. Unserialize and update their state / config from a given property tree.
*
* This is useful for loading object using configuration file, and to save an object's configuration
* into a property tree.
*/
class IXmlSerializable
{
public:
    virtual ~IXmlSerializable() {}

    /**
    * Fill the property tree with relevant attributes.
    */
    virtual void    serialize(ptree& node) = 0;

    /**
    * The implementation shall update the object's status with relevant data from the property tree.
    */
    virtual void    deserialize(const ptree& node) = 0;
};

#endif // IXMLSERIALIZABLE_HPP
