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
public:
    using CSN = std::vector<Byte>;

public:
    explicit Authenticator() = default;
    ~Authenticator() = default;

    Authenticator(const Authenticator& other) = delete;
    Authenticator& operator=(const Authenticator& other) = delete;

    virtual void    serialize(ptree& node) override;
    virtual void    deserialize(const ptree& node) override;

public:
    bool    hasAccess(const CSN& csn);
    void    authorizeCard(const CSN& csn);

public:
    static std::string  serializeCard(const CSN& csn);
    static CSN          deserializeCard(const std::string& card);

private:
    std::list<CSN>  _csnList;
};

#endif // AUTHENTICATOR_HPP
