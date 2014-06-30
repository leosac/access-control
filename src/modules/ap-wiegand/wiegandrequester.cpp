/**
 * \file wiegandrequester.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief WiegandRequester class implementation
 */

#include "wiegandrequester.hpp"

#include <sstream>
#include <iomanip>

#include "hardware/device/wiegandreader.hpp"

WiegandRequester::WiegandRequester(IAccessPointModule& accessPoint, WiegandReader* device)
:   _accessPoint(accessPoint),
    _device(device)
{
    _device->registerListener(this);
}

WiegandRequester::~WiegandRequester()
{
    _device->unregisterListener(this);
}

void WiegandRequester::notifyCardRead(const IWiegandListener::CardId& cardId)
{
    std::ostringstream  oss; // FIXME encode in xml

    for (std::size_t i = 0; i < cardId.size(); ++i)
    {
        if (i > 0)
            oss << ':';
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(cardId[i]);
    }
    _accessPoint.notifyAccess(oss.str());
}
