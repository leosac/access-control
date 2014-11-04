#pragma once

#include <chrono>
#include <memory>
#include <core/auth/Interfaces/IAccessProfile.hpp>

namespace Leosac
{
    namespace Auth
    {
        /**
        * Concrete implementation of a simple access control class.
        */
        class SimpleAccessProfile : public IAccessProfile
        {
        public:
            virtual bool isAccessGranted(const std::chrono::system_clock::time_point &date,
                    const std::string &target) override;
        };
    }
}
