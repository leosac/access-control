/**
 * \file authrequest.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief AuthRequest class declaration
 */

#ifndef AUTHREQUEST_HPP
#define AUTHREQUEST_HPP

#include <cstdint>
#include <string>

class AuthRequest
{
public:
    typedef std::uint32_t Uid;
    enum class Status : std::uint8_t {
        New = 0,
        Accepted,
        Ended
    };

public:
    explicit AuthRequest(const std::string& target, const std::string& requestInfo = std::string());
    ~AuthRequest() = default;

    AuthRequest(const AuthRequest& other);
    AuthRequest& operator=(const AuthRequest& other) = delete;

    AuthRequest(AuthRequest&&) = default; // FIXME

public:
    Uid     getUid() const;
    void    grant(bool granted);
    void    setStatus(Status status);

private:
    static Uid  UidCounter;

private:
    const Uid           _uid;
    const std::string   _target;
    const std::string   _requestInfo;
    Status              _status;
    bool                _granted;
};

#endif // AUTHREQUEST_HPP
