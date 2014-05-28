/**
 * \file authenticator.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Authenticator class declaration
 */

#ifndef AUTHENTICATOR_HPP
#define AUTHENTICATOR_HPP

#include <vector>
#include <list>

#include "config/ixmlserializable.hpp"
#include "tools/bufferutils.hpp"

class Authenticator : public IXmlSerializable
{
    typedef std::vector<Byte>   CSN;

public:
    explicit Authenticator() = default;
    ~Authenticator() = default;

    Authenticator(const Authenticator& other) = delete;
    Authenticator& operator=(const Authenticator& other) = delete;

    virtual void    serialize(boost::property_tree::ptree& node) override;
    virtual void    deserialize(const boost::property_tree::ptree& node) override;

public:
    bool    hasAccess(const CSN& csn);
    void    authorizeCard(const CSN& csn);

private:
    std::string serializeCard(const CSN& csn);
    void        deserializeCard(const std::string& card);

private:
    std::list<CSN>  _csnList;
};

#endif // AUTHENTICATOR_HPP
