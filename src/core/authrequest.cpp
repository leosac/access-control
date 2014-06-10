/**
 * \file authrequest.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief AuthRequest class implementation
 */

#include "authrequest.hpp"

AuthRequest::AuthRequest(Uid id, const std::string& content)
:   _uid(id),
    _content(content),
    _state(0),
    _date(system_clock::now())
{}

bool AuthRequest::operator<(const AuthRequest& other) const
{
    return (_date < other._date);
}

int AuthRequest::getState() const
{
    return (_state);
}

void AuthRequest::setState(int state)
{
    _state = state;
}

AuthRequest::Uid AuthRequest::getId() const
{
    return (_uid);
}

const std::string& AuthRequest::getContent() const
{
    return (_content);
}
