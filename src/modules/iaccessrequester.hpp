/**
 * \file iaccessrequester.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief interface for AccessRequester classes
 * NOTE no function is specified for now, because this class is only supposed
 * to call a function of the associated access point.
 */

#ifndef IACCESSREQUESTER_HPP
#define IACCESSREQUESTER_HPP

class IAccessRequester
{
public:
    virtual ~IAccessRequester() {}
};

#endif // IACCESSREQUESTER_HPP
