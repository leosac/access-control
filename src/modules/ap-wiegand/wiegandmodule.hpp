/**
 * \file wiegandmodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief wiegand module class
 */

#ifndef WIEGANDMODULE_HPP
#define WIEGANDMODULE_HPP

#include "modules/imodule.hpp"

class WiegandModule : public IModule
{
public:
    explicit WiegandModule(IEventListener* listener);
    ~WiegandModule();

    WiegandModule(const WiegandModule& other) = delete;
    WiegandModule& operator=(const WiegandModule& other) = delete;

public:
    void                notify(const Event& event);
    Type                getType() const;
    const std::string&  getVersionString() const;

private:
    IEventListener*     _listener;
    const std::string   _version;
};

#endif // WIEGANDMODULE_HPP
