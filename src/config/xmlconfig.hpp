/**
 * \file xmlconfig.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief XmlConfig class declaration
 */

#ifndef XMLCONFIG_HPP
#define XMLCONFIG_HPP

#include <string>

#include "ixmlserializable.hpp"

class XmlConfig
{
    static const int WhiteSpaceIndent = 4;

public:
    explicit XmlConfig(const std::string& file, IXmlSerializable& object);
    ~XmlConfig() = default;

    XmlConfig(const XmlConfig& other) = delete;
    XmlConfig& operator=(const XmlConfig& other) = delete;

public:
    void                serialize();
    void                deserialize();
    const std::string&  getFilename() const;

private:
    const std::string   _file;
    IXmlSerializable&   _object;
};

#endif // XMLCONFIG_HPP
