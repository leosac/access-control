/**
 * \file authtestmodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief simple auth module
 */

#ifndef AUTHTESTMODULE_HPP
#define AUTHTESTMODULE_HPP

#include "modules/iauthmodule.hpp"
#include "tools/bufferutils.hpp"
#include "authenticator.hpp"

class AuthTestModule : public IAuthModule
{
public:
    typedef std::vector<Byte> CardId;

public:
    explicit AuthTestModule(IEventListener& listener, const std::string& name);
    ~AuthTestModule() noexcept = default;

    AuthTestModule(const AuthTestModule& other) = delete;
    AuthTestModule& operator=(const AuthTestModule& other) = delete;

public:
    virtual void                notify(const Event& event) override;
    virtual const std::string&  getName() const override;
    virtual ModuleType          getType() const override;
    virtual void                serialize(boost::property_tree::ptree& node) override;
    virtual void                deserialize(boost::property_tree::ptree& node) override;

private:
    IEventListener&         _listener;
    Authenticator           _auth;
    const std::string       _name;
};

#endif // AUTHTESTMODULE_HPP
