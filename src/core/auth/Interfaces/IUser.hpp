#pragma once

#include <memory>
#include "IAccessProfile.hpp"

namespace Leosac
{
    namespace Auth
    {
        class IUser;
        using IUserPtr = std::shared_ptr<IUser>;

        /**
        * Represent a user
        */
        class IUser
        {
        public:
            IUser(const std::string &user_id);
            virtual ~IUser() = default;

            /**
            * Get the current id.
            */
            const std::string &id() const noexcept;

            /**
            * Set a new id.
            */
            void id(const std::string &id_new);

            IAccessProfilePtr profile() const noexcept;

            void profile(IAccessProfilePtr user_profile);

        protected:
            /**
            * This is an (unique) identifier for the user.
            */
            std::string id_;

            IAccessProfilePtr profile_;
        };

    }
}
