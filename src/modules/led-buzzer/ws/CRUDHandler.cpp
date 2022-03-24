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

#include "modules/led-buzzer/ws/CRUDHandler.hpp"
#include "exception/EntityNotFound.hpp"
#include "hardware/Buzzer_odb.h"
#include "hardware/LED_odb.h"
#include "hardware/serializers/BuzzerSerializer.hpp"
#include "modules/websock-api/api/APISession.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/db/DBService.hpp"
#include "tools/db/OptionalTransaction.hpp"
#include <hardware/serializers/LEDSerializer.hpp>

namespace Leosac
{
namespace Module
{
namespace LedBuzzer
{
constexpr const char CRUDHandlerHelper::led_object_id_key[];
constexpr const char CRUDHandlerHelper::buzzer_object_id_key[];

template <typename ObjectT, const char *ObjectIdKey, typename SerializerT>
std::vector<WebSockAPI::ICRUDResourceHandler::ActionActionParam>
CRUDHandler<ObjectT, ObjectIdKey, SerializerT>::required_permission(
    WebSockAPI::ICRUDResourceHandler::Verb verb, const json &req) const
{
    std::vector<CRUDResourceHandler::ActionActionParam> ret;

    SecurityContext::HardwareDeviceActionParam hardware_action_param{};
    try
    {
        hardware_action_param.device_id =
            req.at(ObjectIdKey).get<Hardware::DeviceId>();
    }
    catch (json::out_of_range &e)
    {
        hardware_action_param.device_id = Hardware::DeviceId{};
    }

    switch (verb)
    {
    case Verb::READ:
        ret.emplace_back(SecurityContext::Action::HARDWARE_READ,
                         hardware_action_param);
        break;
    case Verb::CREATE:
        ret.emplace_back(SecurityContext::Action::HARDWARE_CREATE,
                         hardware_action_param);
        break;
    case Verb::UPDATE:
        ret.emplace_back(SecurityContext::Action::HARDWARE_UPDATE,
                         hardware_action_param);
        break;
    case Verb::DELETE:
        ret.emplace_back(SecurityContext::Action::HARDWARE_DELETE,
                         hardware_action_param);
        break;
    }
    return ret;
}

template <typename ObjectT, const char *ObjectIdKey, typename SerializerT>
boost::optional<json>
CRUDHandler<ObjectT, ObjectIdKey, SerializerT>::create_impl(const json &req)
{
    json rep;
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    auto new_led_or_buzzer = std::make_shared<ObjectT>();
    SerializerT::unserialize(*new_led_or_buzzer, req.at("attributes"),
                             security_context());
    db->persist(new_led_or_buzzer);

    rep["data"] = SerializerT::serialize(*new_led_or_buzzer, security_context());
    t.commit();
    return rep;
}


// Declare function to retrieve either led or buzzer.
// We need to specialize this template to be able to throw
// a correct EntityNotFound exception (because object's type is a string)
template <typename T>
std::shared_ptr<T> find_led_or_buzzer_by_id(const Hardware::DeviceId &id, DBPtr db);

template <>
Hardware::LEDPtr
find_led_or_buzzer_by_id<Hardware::LED>(const Hardware::DeviceId &id, DBPtr db)
{
    db::OptionalTransaction t(db->begin());
    auto led = db->find<Hardware::LED>(id);
    t.commit();
    if (!led)
        throw EntityNotFound(id, "led");
    return led;
}

template <>
Hardware::BuzzerPtr
find_led_or_buzzer_by_id<Hardware::Buzzer>(const Hardware::DeviceId &id, DBPtr db)
{
    db::OptionalTransaction t(db->begin());
    auto buzzer = db->find<Hardware::Buzzer>(id);
    t.commit();
    if (!buzzer)
        throw EntityNotFound(id, "buzzer");
    return buzzer;
}

template <typename ObjectT, const char *ObjectIdKey, typename SerializerT>
boost::optional<json>
CRUDHandler<ObjectT, ObjectIdKey, SerializerT>::read_impl(const json &req)
{
    json rep;

    using Result = odb::result<ObjectT>;
    DBPtr db     = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto led_or_buzzer_id = req.at(ObjectIdKey).get<Hardware::DeviceId>();

    if (!led_or_buzzer_id.is_nil())
    {
        auto led_or_buzzer = find_led_or_buzzer_by_id<ObjectT>(led_or_buzzer_id, db);
        rep["data"] = SerializerT::serialize(*led_or_buzzer, security_context());
    }
    else
    {
        Result result     = db->query<ObjectT>();
        rep["data"]       = json::array();
        auto current_user = ctx_.session->current_user();

        // fixme: may be rather slow.
        for (const auto &led_or_buzzer : result)
        {
            SecurityContext::HardwareDeviceActionParam hap{.device_id =
                                                               led_or_buzzer.id()};
            if (ctx_.session->security_context().check_permission(
                    SecurityContext::Action::HARDWARE_READ, hap))
            {
                rep["data"].push_back(
                    SerializerT::serialize(led_or_buzzer, security_context()));
            }
        }
    }
    t.commit();
    return rep;
}

template <typename ObjectT, const char *ObjectIdKey, typename SerializerT>
boost::optional<json>
CRUDHandler<ObjectT, ObjectIdKey, SerializerT>::update_impl(const json &req)
{
    json rep;
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto led_or_buzzer_id = req.at(ObjectIdKey).get<Hardware::DeviceId>();
    auto led_or_buzzer    = find_led_or_buzzer_by_id<ObjectT>(led_or_buzzer_id, db);

    SerializerT::unserialize(*led_or_buzzer, req.at("attributes"),
                             security_context());

    db->update(led_or_buzzer);
    rep["data"] = SerializerT::serialize(*led_or_buzzer, security_context());
    t.commit();
    return rep;
}

template <typename ObjectT, const char *ObjectIdKey, typename SerializerT>
boost::optional<json>
CRUDHandler<ObjectT, ObjectIdKey, SerializerT>::delete_impl(const json &req)
{
    auto did = req.at(ObjectIdKey).get<Hardware::DeviceId>();
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    auto led_or_buzzer = find_led_or_buzzer_by_id<ObjectT>(did, db);
    db->erase(led_or_buzzer);
    t.commit();

    return json{};
}

template <typename ObjectT, const char *ObjectIdKey, typename SerializerT>
CRUDHandler<ObjectT, ObjectIdKey, SerializerT>::CRUDHandler(
    const WebSockAPI::RequestContext &ctx)
    : CRUDResourceHandler(ctx)
{
    static_assert(std::is_same<ObjectT, Hardware::LED>::value ||
                      std::is_same<ObjectT, Hardware::Buzzer>::value,
                  "This CRUDHandler can only work with either LED or Buzzer type");
}

template <typename ObjectT, const char *ObjectIdKey, typename SerializerT>
WebSockAPI::CRUDResourceHandlerUPtr
CRUDHandler<ObjectT, ObjectIdKey, SerializerT>::instanciate(
    WebSockAPI::RequestContext ctx)
{
    auto instance = WebSockAPI::CRUDResourceHandlerUPtr(
        new CRUDHandler<ObjectT, ObjectIdKey, SerializerT>(ctx));
    return instance;
}

template class CRUDHandler<Hardware::LED, CRUDHandlerHelper::led_object_id_key,
                           Hardware::LEDSerializer>;

template class CRUDHandler<Hardware::Buzzer, CRUDHandlerHelper::buzzer_object_id_key,
                           Hardware::BuzzerSerializer>;
}
}
}
