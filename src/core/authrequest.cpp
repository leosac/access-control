/**
 * \file authrequest.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief AuthRequest class implementation
 */

#include "authrequest.hpp"

AuthRequest::AuthRequest(Uid id, const std::string& content, const std::string& target)
:   _uid(id),
    _content(content),
    _target(target),
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

const std::string& AuthRequest::getTarget() const
{
    return (_target);
}

void AuthRequest::setTarget(const std::string& target)
{
    _target = target;
}

AuthRequest::Uid AuthRequest::getId() const
{
    return (_uid);
}

const std::string& AuthRequest::getContent() const
{
    return (_content);
}
