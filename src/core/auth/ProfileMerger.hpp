/*
    Copyright (C) 2014-2015 Islog

    This file is part of Leosac.

    Leosac is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leosac is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

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
