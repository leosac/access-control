/*
    Copyright (C) 2014-2022 Leosac

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

#include "tools/log.hpp"
#include <memory>

namespace Leosac
{

template <typename T>
struct is_shared_ptr : public std::false_type
{
};

template <typename T>
struct is_shared_ptr<std::shared_ptr<T>> : public std::true_type
{
};

template <typename T>
struct is_shared_ptr<const std::shared_ptr<T>> : public std::true_type
{
};

template <typename T>
struct is_shared_ptr<std::shared_ptr<T> &> : public std::true_type
{
};

template <typename T>
struct is_shared_ptr<const std::shared_ptr<T> &> : public std::true_type
{
};

template <typename T>
constexpr bool is_shared_ptr_v = is_shared_ptr<T>::value;

template <typename Out, typename In>
std::enable_if_t<is_shared_ptr_v<Out>, Out>
assert_cast(const std::shared_ptr<In> &in)
{
    bool is_null = !in;

    if (auto p = dynamic_cast<typename Out::element_type *>(in.get()))
    {
        return Out(in, p);
    }
    else if (!is_null)
    {
        ASSERT_LOG(0, "Pointer is null after assert_cast");
    }
    return nullptr;
};

template <typename Out, typename In>
std::enable_if_t<!is_shared_ptr_v<Out> && !std::is_pointer<Out>::value, Out &&>
assert_cast(In &&in)
{
    try
    {
        Out &&tmp = dynamic_cast<Out &&>(in);
        return std::forward<Out>(tmp);
    }
    catch (const std::bad_cast &)
    {
        ASSERT_LOG(false, "Cast failed.");
    }
    std::terminate();
};

template <typename Out, typename In>
std::enable_if_t<std::is_pointer<Out>::value && std::is_pointer<In>::value, Out>
assert_cast(In &&in)
{
    bool is_null = !in;

    if (auto p = dynamic_cast<Out>(in))
    {
        return p;
    }
    else if (!is_null)
    {
        ASSERT_LOG(0, "Pointer is null after assert_cast");
    }
    return nullptr;
};
}
