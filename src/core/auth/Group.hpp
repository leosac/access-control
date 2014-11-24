#pragma once

#include <string>
#include <vector>
#include "core/auth/Interfaces/IUser.hpp"

namespace Leosac
{
    namespace Auth
    {

        class Group;
        using GroupPtr = std::shared_ptr<Group>;
        /**
        * A authentication group regroup users that share permissions.
        */
        class Group
        {
        public:
            explicit Group(const std::string &group_name);

            const std::string &name() const;

            const std::vector<IUserPtr> &members() const;

            void member_add(IUserPtr m);

            IAccessProfilePtr profile();

            void profile(IAccessProfilePtr p);

        private:
            std::vector<IUserPtr> members_;
            std::string name_;
            IAccessProfilePtr profile_;
        };
    }
}