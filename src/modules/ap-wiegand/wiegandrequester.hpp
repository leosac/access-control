/**
 * \file wiegandrequester.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief WiegandRequester class declaration
 */

#ifndef WIEGANDREQUESTER_HPP
#define WIEGANDREQUESTER_HPP

#include "modules/iaccessrequester.hpp"

class WiegandRequester : public IAccessRequester
{
public:
    explicit WiegandRequester() = default;
    ~WiegandRequester() = default;

    WiegandRequester(const WiegandRequester& other) = delete;
    WiegandRequester& operator=(const WiegandRequester& other) = delete;
};

#endif // WIEGANDREQUESTER_HPP
