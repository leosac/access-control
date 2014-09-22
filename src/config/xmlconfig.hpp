/**
 * \file xmlconfig.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief XmlConfig class declaration
 */

#ifndef XMLCONFIG_HPP
#define XMLCONFIG_HPP

#include <string>

#include "ixmlserializable.hpp"

/**
* Wrapper around an XML configuration file associated with an IXmlSerializable object.
*
* This glass glues together a configuration file (retrieve from path) and an object implementing the
* IXmlSerializable interface.
*/
class XmlConfig
{
    static const int WhiteSpaceIndent = 4;

public:
    /**
    * Create a XmlConfig object using a target file path and an associated IXmlSerializable object.
    * @param file : path to the configuration on the filesystem.
    * @param object : associated object that will be serialized to / deserialized from the config object.
    */
    explicit XmlConfig(const std::string& file, IXmlSerializable& object);
    ~XmlConfig() = default;

    XmlConfig(const XmlConfig& other) = delete;
    XmlConfig& operator=(const XmlConfig& other) = delete;

public:
    /**
    * Read the file from disk, extract a meaningful property tree from the data, and call the
    * IXmlSerializable::deserialize() method on the associated object.
    * @throws ConfigException if it failed to either read or parse the configuration file.
    */
    void                load();

    /**
    * Build a property tree by calling IXmlSerializable::serialize() on the associated object, and then
    * write this tree to file using the XML format.
    * @throws ConfigException if it failed to either write the configuration file or build the tree.
    */
    void                save();

    /**
    * Return the name of the file we read from / write to.
    */
    const std::string&  getFilename() const;

private:
    const std::string   _file;
    IXmlSerializable&   _object;
};

#endif // XMLCONFIG_HPP
