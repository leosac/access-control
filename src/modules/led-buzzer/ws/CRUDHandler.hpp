/*
    Copyright (C) 2014-2017 Leosac

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

#include "modules/websock-api/api/CRUDResourceHandler.hpp"

namespace Leosac
{
namespace Module
{
namespace LedBuzzer
{

struct CRUDHandlerHelper
{
    /**
     * How to retrieve the object's id when instanciate the crud handler for buzzer
     */
    static constexpr const char buzzer_object_id_key[] = "buzzer_id";
    /**
     * How to retrieve the object's id when instanciate the crud handler for led
     */
    static constexpr const char led_object_id_key[] = "led_id";
};

/**
 * CRUD handler for Buzzer and LED.
 *
 * Since LED and Buzzer are very close wrt their attributes
 * we use a templated CRUD Handler.
 *
 * @tparam ObjectT LED or Buzzer
 * @tparam ObjectIdKey  The key in JSON to retrieve object's id.
 * @tparam SerializerT The serializer type to use.
 */
template <typename ObjectT, const char *ObjectIdKey, typename SerializerT>
class CRUDHandler : public WebSockAPI::CRUDResourceHandler
{
  public:
    explicit CRUDHandler(const WebSockAPI::RequestContext &ctx);

    static WebSockAPI::CRUDResourceHandlerUPtr
        instanciate(WebSockAPI::RequestContext);

    std::vector<ActionActionParam>
    required_permission(Verb verb, const WebSockAPI::json &req) const override;

    boost::optional<WebSockAPI::json>
    create_impl(const WebSockAPI::json &req) override;

    boost::optional<WebSockAPI::json>
    read_impl(const WebSockAPI::json &req) override;

    boost::optional<WebSockAPI::json>
    update_impl(const WebSockAPI::json &req) override;

    boost::optional<WebSockAPI::json>
    delete_impl(const WebSockAPI::json &req) override;
};
}
}
}
