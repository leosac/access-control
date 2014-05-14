/**
 * \file authrequest.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief AuthRequest class implementation
 */

#include "authrequest.hpp"

AuthRequest::Uid AuthRequest::UidCounter = 0;

AuthRequest::AuthRequest(const std::string& target, const std::string& requestInfo)
:   _uid(UidCounter),
    _target(target),
    _requestInfo(requestInfo),
    _status(Status::New),
    _granted(false)
{
    ++UidCounter;
}

AuthRequest::AuthRequest(const AuthRequest& other)
:   _uid(other._uid),
    _target(other._target),
    _requestInfo(other._requestInfo),
    _status(other._status),
    _granted(other._granted)
{}

AuthRequest::Uid AuthRequest::getUid() const
{
    return (_uid);
}

void AuthRequest::grant(bool granted)
{
    _granted = granted;
    _status = Status::Accepted;
}

void AuthRequest::setStatus(AuthRequest::Status status)
{
    _status = status;
}
