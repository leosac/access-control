#include <core/auth/Interfaces/IAccessProfile.hpp>

#pragma once

namespace Leosac
{
    namespace Auth
    {
        /**
        * Helper class that merges profile together.
        *
        * This is useful to build a profile for a user that is in multiple group.
        *
        * Ideally this class could use a strategy-like pattern, but since we only have
        * one way to merge profile yet, we wont.
        */
        class ProfileMerger
        {
        public:
            /**
            * Build a new Profile object by merging two profiles.
            * @warn works only with SimpleAccessProfile object.
            */
            virtual IAccessProfilePtr merge(std::shared_ptr<const IAccessProfile> p1,
                    std::shared_ptr<const IAccessProfile> p2);
        };
    }
}
