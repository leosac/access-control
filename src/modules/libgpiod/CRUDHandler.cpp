/*
    Copyright (C) 2014-2025 Leosac

    This file is part of Leosac Access Control.

    Leosac Access Control is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leosac Access Control is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "modules/libgpiod/LibgpiodGPIO_odb.h"
#include <boost/uuid/string_generator.hpp>
#include "CRUDHandler.hpp"
#include "LibgpiodGPIO.hpp"
#include "LibgpiodGPIOSerializer.hpp"
#include "modules/libgpiod/LibgpiodGPIO_odb.h"
#include "modules/websock-api/api/APISession.hpp"
#include "tools/db/DBService.hpp"
#include "tools/db/OptionalTransaction.hpp"
#include <odb/transaction.hxx>
#include <odb/schema-catalog.hxx>
#include <exception/EntityNotFound.hpp>
#include "exception/ModelException.hpp"
#include <tools/JSONUtils.hpp>

namespace Leosac {
namespace Module {
namespace Libgpiod {

CRUDHandler::CRUDHandler(const WebSockAPI::RequestContext &ctx)
    : WebSockAPI::CRUDResourceHandler(ctx) {}

WebSockAPI::CRUDResourceHandlerUPtr CRUDHandler::instanciate(WebSockAPI::RequestContext ctx) {
    return std::make_unique<CRUDHandler>(ctx);
}

std::vector<WebSockAPI::ICRUDResourceHandler::ActionActionParam> CRUDHandler::required_permission(Verb verb, const WebSockAPI::json &req) const {
    // For simplicity, require admin for all actions
    return {{SecurityContext::Action::IS_ADMIN, {}}};
}

void CRUDHandler::validate_uniqueness(const std::string& name, const std::string& device) {
    auto db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    // ODB query for name and device uniqueness
    auto r = db->query<LibgpiodGPIO>(odb::query<LibgpiodGPIO>::name == name && odb::query<LibgpiodGPIO>::device == device);
    if (!r.empty()) {
        throw ModelException("", "A GPIO with the same name and device already exists.");
    }
}

boost::optional<WebSockAPI::json> CRUDHandler::create_impl(const WebSockAPI::json &req) {
    LibgpiodGPIO gpio;
    LibgpiodGPIOSerializer::unserialize(gpio, req, ctx_.security_ctx);
    validate_uniqueness(gpio.name(), gpio.device());
    auto db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    db->persist(gpio);
    t.commit();
    return LibgpiodGPIOSerializer::serialize(gpio, ctx_.security_ctx);
}

boost::optional<WebSockAPI::json> CRUDHandler::read_impl(const WebSockAPI::json &req) {
    auto db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    if (req.contains("id")) {
    auto uuid = boost::uuids::string_generator()(req.at("id").get<std::string>());
    auto gpio = db->find<LibgpiodGPIO>(Leosac::UUID(uuid));
        if (!gpio) throw EntityNotFound(req.at("id").get<unsigned long>(), "libgpiod.gpio");
        return LibgpiodGPIOSerializer::serialize(*gpio, ctx_.security_ctx);
    } else {
        // List all
    odb::result<LibgpiodGPIO> r = db->query<LibgpiodGPIO>();
        nlohmann::json arr = nlohmann::json::array();
        for (const auto& gpio : r) {
            arr.push_back(LibgpiodGPIOSerializer::serialize(gpio, ctx_.security_ctx));
        }
        return arr;
    }
}

boost::optional<WebSockAPI::json> CRUDHandler::update_impl(const WebSockAPI::json &req) {
    if (!req.contains("id")) throw ModelException("", "Missing id for update");
    auto db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto uuid = boost::uuids::string_generator()(req.at("id").get<std::string>());
    auto gpio = db->find<LibgpiodGPIO>(Leosac::UUID(uuid));
    if (!gpio) throw EntityNotFound(req.at("id").get<unsigned long>(), "libgpiod.gpio");
    std::string new_name = req.value("name", gpio->name());
    std::string new_device = req.value("device", gpio->device());
    if (new_name != gpio->name() || new_device != gpio->device()) {
        validate_uniqueness(new_name, new_device);
    }
    LibgpiodGPIOSerializer::unserialize(*gpio, req, ctx_.security_ctx);
    db->update(*gpio);
    t.commit();
    return LibgpiodGPIOSerializer::serialize(*gpio, ctx_.security_ctx);
}

boost::optional<WebSockAPI::json> CRUDHandler::delete_impl(const WebSockAPI::json &req) {
    if (!req.contains("id")) throw ModelException("", "Missing id for delete");
    auto db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    auto uuid = boost::uuids::string_generator()(req.at("id").get<std::string>());
    auto gpio = db->find<LibgpiodGPIO>(Leosac::UUID(uuid));
    if (!gpio) throw EntityNotFound(req.at("id").get<unsigned long>(), "libgpiod.gpio");
    db->erase(*gpio);
    t.commit();
    return {};
}

}
}
}
