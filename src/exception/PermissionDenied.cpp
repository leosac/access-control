/*
    Copyright (C) 2014-2016 Leosac

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

#include "PermissionDenied.hpp"

PermissionDenied::PermissionDenied(const std::string &detail)
    : LEOSACException(build_msg(detail))
{
}

std::string PermissionDenied::build_msg(const std::string &detail) const
{
    std::stringstream ss;
    ss << "Permission denied";

    if (detail.size())
        ss << ": " << detail;
    else
        ss << ".";

    return ss.str();
}
