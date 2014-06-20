/**
 * \file authrequest.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief AuthRequest class implementation
 */

#include "authrequest.hpp"

AuthRequest::AuthRequest(Uid id, const std::string& source, const std::string& target, const std::string& content)
:   _uid(id),
    _source(source),
    _target(target),
    _content(content),
    _state(AuthRequest::New),
    _date(system_clock::now())
{}

bool AuthRequest::operator<(const AuthRequest& other) const
{
    return (_date < other._date);
}

AuthRequest::Uid AuthRequest::getId() const
{
    return (_uid);
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

const std::string& AuthRequest::getSource() const
{
    return (_source);
}

const std::string& AuthRequest::getContent() const
{
    return (_content);
}

system_clock::time_point AuthRequest::getDate()
{
    return (_date);
}
