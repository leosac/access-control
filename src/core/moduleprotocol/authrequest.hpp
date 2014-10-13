/**
 * \file authrequest.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief AuthRequest class declaration
 */

#ifndef AUTHREQUEST_HPP
#define AUTHREQUEST_HPP

#include <chrono>
#include <string>

using std::chrono::system_clock;

class AuthRequest
{
public:
    using Uid = unsigned long;
    enum State {
        New = 1, // NOTE 0 is used as error value for DFAutomata
        Waiting,
        AskAuth,
        Authorized,
        Denied,
        Closed
    };

public:
    explicit AuthRequest(Uid id, const std::string& source, const std::string& target, const std::string& content);
    ~AuthRequest() = default;

    AuthRequest(const AuthRequest& other) = default;
    AuthRequest& operator=(const AuthRequest& other) = default;

public:
    bool                        operator<(const AuthRequest& other) const;
    Uid                         getId() const;
    int                         getState() const;
    void                        setState(int state);

    /**
    * Returns the targeted device, meaning the device we are trying to access (door)
    */
    const std::string&          getTarget() const;

    /**
    * Returns the source of the auth request, this should be the name of the access point that initiated the request.
    */
    const std::string&          getSource() const;
    const std::string&          getContent() const;
    system_clock::time_point    getTime() const;

private:
    Uid                         _uid;
    std::string                 _source;
    std::string                 _target;
    std::string                 _content;
    int                         _state;
    system_clock::time_point    _time;
};

#endif // AUTHREQUEST_HPP
