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
#include "WebSockFwd.hpp"
#include "core/SecurityContext.hpp"
#include <json.hpp>
#include <vector>

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
using json = nlohmann::json;

class CRUDResourceHandler
{
  protected:
    CRUDResourceHandler(RequestContext ctx);

  public:
    enum class Verb
    {
        CREATE,
        READ,
        UPDATE,
        DELETE
    };
    using Factory = CRUDResourceHandlerUPtr (*)(RequestContext);

    static CRUDResourceHandlerUPtr instanciate(RequestContext);

    json process(const ClientMessage &msg);

    /**
     * Add a bunch of condition: at least one of them must return
     * true, otherwise `on_failure()` will be invoked.
     *
     * @param on_failure Invoked if all condition returned false.
     * @param conditions A bunch of condition-like object.
     */
    template <typename OnFailureT, typename... T>
    void add_conditions_or(Verb verb, const OnFailureT &on_failure,
                           const T &... conditions)
    {
        ConditionGroup new_conditions{conditions...};
        if (verb == Verb::READ)
            read_conditions_.push_back(std::make_pair(new_conditions, on_failure));
        else if (verb == Verb::CREATE)
            create_conditions_.push_back(std::make_pair(new_conditions, on_failure));
        else if (verb == Verb::UPDATE)
            update_conditions_.push_back(std::make_pair(new_conditions, on_failure));
        else if (verb == Verb::DELETE)
            delete_conditions_.push_back(std::make_pair(new_conditions, on_failure));
        else
        {
            assert(0 && "Should not be here");
        }
    };

    /**
     * A group of conditions. At least one of those condition
     * must PASS in order for the condition group to PASS.
     */
    using ConditionGroup = std::vector<std::function<bool(const json &)>>;

    using ConditionGroupVector =
        std::vector<std::pair<ConditionGroup, std::function<void()>>>;

  protected:
    /**
     * A pair of Action and a generic ActionParam union.
     */
    using ActionActionParam =
        std::pair<SecurityContext::Action, SecurityContext::ActionParam>;

    RequestContext ctx_;

    /**
     * Condition that are evaluated before handling the request.
     */
    ConditionGroupVector read_conditions_;
    ConditionGroupVector update_conditions_;
    ConditionGroupVector create_conditions_;
    ConditionGroupVector delete_conditions_;

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

    /**
     * Helper function that returns the security context.
     */
    SecurityContext &security_context();

  private:
    virtual std::vector<ActionActionParam>
    required_permission(Verb verb, const json &req) const = 0;


    virtual json create_impl(const json &req) = 0;

    virtual json read_impl(const json &req) = 0;

    virtual json update_impl(const json &req) = 0;

    virtual json delete_impl(const json &req) = 0;

    Verb verb_from_request_type(const std::string &);
    void enforce_condition(const ConditionGroupVector &conditions,
                           const json &msg_content);

    void enforce_permission(const std::vector<ActionActionParam> &);
};
}
}
}
