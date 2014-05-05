/**
 * \file unlock_guard.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief custom std equivalent for boost::unlock_guard
 */

#ifndef UNLOCK_GUARD_HPP
#define UNLOCK_GUARD_HPP

template <class T>
class unlock_guard
{
public:
    explicit unlock_guard(T& mutex) : _mutex(mutex) {
        _mutex.unlock();
    }

    ~unlock_guard() {
        _mutex.lock();
    }

    unlock_guard(const unlock_guard& other) = delete;
    unlock_guard& operator=(const unlock_guard& other) = delete;

private:
    T&  _mutex;
};

#endif // UNLOCK_GUARD_HPP
