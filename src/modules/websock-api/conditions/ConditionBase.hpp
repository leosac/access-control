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

#include "RequestContext.hpp"
#include <json.hpp>

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
using json = nlohmann::json;

namespace Conditions
{
/**
 * The base class for request Condition.
 *
 * A combination of Condition is used to determine wether or not a request
 * should be processed by the Leosac server.
 *
 * The Condition infrastructure should be flexible to be able to make a decision
 * using various factor, such as:
 *     + User Permission
 *     + User group membership
 *     + Rate limiting
 *     + ...
 *
 * To be considered a condition, an object should provide
 *    `operator()(const json&)`. However, if the condition do not plan to use
 *    the json request object, it can call `wrap()`.
 */
class ConditionBase
{
  public:
    ConditionBase(RequestContext rctx);

  protected:
    RequestContext ctx_;
};

/**
 * A wrapper object for Condition that do not want the
 * addition `const json &` request object.
 */
template <typename T>
struct NoParamWrapper
{
    NoParamWrapper(const T &o)
        : original(o)
    {
    }

    bool operator()(const json &)
    {
        return original();
    }

  private:
    T original;
};

/**
 * Wrap the condition object to forward the call to
 *  `operator()(const json &)` to `operator()()`
 */
template <typename T>
static NoParamWrapper<T> wrap(const T &o)
{
    static_assert(std::is_base_of<ConditionBase, T>::value,
                  "Object T is not derived from ConditionBase. Refusing to wrap.");
    return NoParamWrapper<T>(o);
}
}
}
}
}
