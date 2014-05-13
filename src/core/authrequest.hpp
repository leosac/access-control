/**
 * \file authrequest.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief AuthRequest class declaration
 */

#ifndef AUTHREQUEST_HPP
#define AUTHREQUEST_HPP

#include <cstdint>

class AuthRequest
{
public:
    typedef std::uint32_t Uid;

public:
    explicit AuthRequest();
    ~AuthRequest() = default;

    AuthRequest(const AuthRequest& other) = delete;
    AuthRequest& operator=(const AuthRequest& other) = delete;

public:
    Uid getUid() const;

private:
    static Uid  UidCounter;

private:
    const Uid   _uid;
};

#endif // AUTHREQUEST_HPP
