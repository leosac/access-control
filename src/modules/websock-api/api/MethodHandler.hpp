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

/**
 * The base class for API method handler implementation.
 *
 * The class defines the very simple interface that must be implemented
 * by the various API method implementation.
 *
 * Each subclass should provide a static method to instanciate a handler
 * object of the subclass type.
 */
class MethodHandler
{
  public:
    using Factory = MethodHandlerUPtr (*)(RequestContext);

    MethodHandler(RequestContext ctx)
        : ctx_(ctx)
    {
    }

    virtual ~MethodHandler() = default;

    /**
     * The public `process()` method.
     *
     * It uses Non Virtual Interface pattern and will forward
     * the call to the `process_impl()` method.
     */
    json process(const ClientMessage &msg);

    /**
     * An example method that should be implemented in all subclasses.
     * Required request-conditions should be created in this method too.
     */
    static MethodHandlerUPtr create(RequestContext)
    {
        return nullptr;
    }

  private:
    /**
     * The API method implementation.
     *
     * @return A json object that will be assigned to the `content` key
     * in the JSON message sent to the client.
     */
    virtual json process_impl(const json &req) = 0;

  protected:
    /**
     * Extract the value of a key from a json object.
     *
     * If the key cannot be found, this function returns the default
     * value instead.
     */
    template <typename T>
    typename std::enable_if<!std::is_same<const char *, T>::value, T>::type
    extract_with_default(const json &obj, const std::string &key, T default_value)
    {
        T ret = default_value;
        try
        {
            ret = obj.at(key).get<T>();
        }
        catch (const std::out_of_range &e)
        {
        }
        return ret;
    }

    template <typename T>
    typename std::enable_if<std::is_same<const char *, T>::value, std::string>::type
    extract_with_default(const json &obj, const std::string &key, T default_value)
    {
        return extract_with_default<std::string>(obj, key, default_value);
    }

    RequestContext ctx_;

    /**
     * Add a bunch of condition: at least one of them must return
     * true, otherwise `on_failure()` will be invoked.
     *
     * @param on_failure Invoked if all condition returned false.
     * @param conditions A bunch of condition-like object.
     */
    template <typename OnFailureT, typename... T>
    void add_conditions_or(const OnFailureT &on_failure, const T &... conditions)
    {
        ConditionVector new_conditions{conditions...};
        conditions_.push_back(std::make_pair(new_conditions, on_failure));
    };

    using ConditionVector = std::vector<std::function<bool(const json &req)>>;

    /**
     * Condition that are evaluated before handling the request.
     */
    std::vector<std::pair<ConditionVector, std::function<void()>>> conditions_;
};
}
}
}
