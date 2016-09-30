/*
    Copyright (C) 2014-2016 Islog

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

#pragma once

namespace Leosac
{
template <typename Out, typename In>
Out assert_cast(In in)
{
    bool is_null = in;
    Out tmp      = dynamic_cast<Out>(in);
    if (!is_null)
        ASSERT_LOG(tmp != nullptr, "Pointer is null after assert_cast");
    return tmp;
};

template <typename Out, typename In>
Out assert_cast(const std::shared_ptr<In> &in)
{
    bool is_null = in;

    if (auto p = dynamic_cast<typename Out::element_type *>(in.get()))
    {
        return Out(in, p);
    }
    else if (is_null)
    {
        ASSERT_LOG(0, "Pointer is null after assert_cast");
    }
    return nullptr;
};
}
