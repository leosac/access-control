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


#include "modules/pifacedigital/CRUDHandler.hpp"
#include "PFGPIO.hpp"
#include "exception/EntityNotFound.hpp"
#include "modules/pifacedigital/PFGPIO_odb.h"
#include "modules/websock-api/api/APISession.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/db/DBService.hpp"
#include "tools/db/OptionalTransaction.hpp"

namespace Leosac
{
namespace Module
{
namespace Piface
{
std::vector<WebSockAPI::ICRUDResourceHandler::ActionActionParam>
CRUDHandler::required_permission(WebSockAPI::ICRUDResourceHandler::Verb verb,
                                 const json &req) const
{
    std::vector<CRUDResourceHandler::ActionActionParam> ret;

    SecurityContext::HardwareDeviceActionParam gpio_action_param;
    try
    {
        gpio_action_param.device_id = req.at("gpio_id").get<Hardware::DeviceId>();
    }
    catch (json::out_of_range &e)
    {
        gpio_action_param.device_id = UUID{};
    }

    switch (verb)
    {
    case Verb::READ:
        ret.emplace_back(SecurityContext::Action::HARDWARE_READ, gpio_action_param);
        break;
    case Verb::CREATE:
        ret.emplace_back(SecurityContext::Action::HARDWARE_CREATE,
                         gpio_action_param);
        break;
    case Verb::UPDATE:
        ret.emplace_back(SecurityContext::Action::HARDWARE_UPDATE,
                         gpio_action_param);
        break;
    case Verb::DELETE:
        ret.emplace_back(SecurityContext::Action::HARDWARE_DELETE,
                         gpio_action_param);
        break;
    }
    return ret;
}

boost::optional<json> CRUDHandler::create_impl(const json &req)
{
    json rep;
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    auto new_gpio = std::make_shared<PFGPIO>();
    PFGPIOSerializer::unserialize(*new_gpio, req.at("attributes"),
                                  security_context());
    db->persist(new_gpio);

    rep["data"] = PFGPIOSerializer::serialize(*new_gpio, security_context());
    t.commit();
    return rep;
}

auto find_gpio_by_id(const Hardware::DeviceId &id, DBPtr db)
{
    db::OptionalTransaction t(db->begin());
    auto gpio = db->find<PFGPIO>(id);
    t.commit();
    if (!gpio)
        throw EntityNotFound(id, "pfdigital.gpio");
    return gpio;
}

boost::optional<json> CRUDHandler::read_impl(const json &req)
{
    json rep;

    using Result = odb::result<PFGPIO>;
    DBPtr db     = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto gpio_id = req.at("gpio_id").get<Hardware::DeviceId>();

    if (!gpio_id.is_nil())
    {
        auto gpio   = find_gpio_by_id(gpio_id, db);
        rep["data"] = PFGPIOSerializer::serialize(*gpio, security_context());
    }
    else
    {
        Result result     = db->query<PFGPIO>();
        rep["data"]       = json::array();
        auto current_user = ctx_.session->current_user();

        // fixme: may be rather slow.
        for (const auto &gpio : result)
        {
            SecurityContext::HardwareDeviceActionParam ap{.device_id = gpio.id()};
            if (ctx_.session->security_context().check_permission(
                    SecurityContext::Action::HARDWARE_READ, ap))
            {
                rep["data"].push_back(
                    PFGPIOSerializer::serialize(gpio, security_context()));
            }
        }
    }
    t.commit();
    return rep;
}

boost::optional<json> CRUDHandler::update_impl(const json &req)
{
    json rep;
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto gpio_id = req.at("gpio_id").get<Hardware::DeviceId>();
    auto gpio    = find_gpio_by_id(gpio_id, db);

    PFGPIOSerializer::unserialize(*gpio, req.at("attributes"), security_context());

    db->update(gpio);
    rep["data"] = PFGPIOSerializer::serialize(*gpio, security_context());
    t.commit();
    return rep;
}

boost::optional<json> CRUDHandler::delete_impl(const json &req)
{
    auto did = req.at("gpio_id").get<Hardware::DeviceId>();
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());

    auto gpio = find_gpio_by_id(did, db);
    db->erase(gpio);
    t.commit();

    return json{};
}

CRUDHandler::CRUDHandler(const WebSockAPI::RequestContext &ctx)
    : CRUDResourceHandler(ctx)
{
}

WebSockAPI::CRUDResourceHandlerUPtr
CRUDHandler::instanciate(WebSockAPI::RequestContext ctx)
{
    auto instance = WebSockAPI::CRUDResourceHandlerUPtr(new CRUDHandler(ctx));
    return instance;
}
}
}
}
