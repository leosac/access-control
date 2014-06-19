/**
 * \file wiegandrequester.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief WiegandRequester class declaration
 */

#ifndef WIEGANDREQUESTER_HPP
#define WIEGANDREQUESTER_HPP

#include "hardware/device/iwiegandlistener.hpp"
#include "modules/iaccesspointmodule.hpp"

class WiegandReader;

class WiegandRequester : public IWiegandListener
{
public:
    explicit WiegandRequester(IAccessPointModule& accessPoint, WiegandReader* device);
    ~WiegandRequester();

    WiegandRequester(const WiegandRequester& other) = default; // FIXME Use move constructor
    WiegandRequester& operator=(const WiegandRequester& other) = default; // FIXME Use move constructor

public:
    virtual void    notifyCardRead(const CardId& cardId) override;

private:
    IAccessPointModule& _accessPoint;
    WiegandReader*      _device;
};

#endif // WIEGANDREQUESTER_HPP
