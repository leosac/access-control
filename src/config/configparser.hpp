/**
 * \file configparser.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief ConfigParser class declaration
 */

#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <istream>

class ConfigParser
{
public:
    explicit ConfigParser() = default;
    ~ConfigParser() = default;

    ConfigParser(const ConfigParser& other) = delete;
    ConfigParser& operator=(const ConfigParser& other) = delete;

public:
    void    parse(std::istream& content);
};

#endif // CONFIGPARSER_HPP
