/**
 * \file authenticator.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Authenticator class declaration
 */

#ifndef AUTHENTICATOR_HPP
#define AUTHENTICATOR_HPP

#include "config/ixmlserializable.hpp"
#include "tools/bufferutils.hpp"

class Authenticator : public IXmlSerializable
{
public:
    explicit Authenticator() = default;
    ~Authenticator() = default;

    Authenticator(const Authenticator& other) = delete;
    Authenticator& operator=(const Authenticator& other) = delete;

    virtual void    serialize(boost::property_tree::ptree& node) override;
    virtual void    deserialize(boost::property_tree::ptree& node) override;

    bool    hasAccess(const std::vector<Byte>& card);

private:

};

#endif // AUTHENTICATOR_HPP
