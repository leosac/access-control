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

#include <boost/optional.hpp>
#include <nlohmann/json.hpp>
#include <memory>

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
class APISession;
using APIPtr = std::shared_ptr<APISession>;

class MethodHandler;
using MethodHandlerUPtr = std::unique_ptr<MethodHandler>;

class CRUDResourceHandler;
using CRUDResourceHandlerUPtr = std::unique_ptr<CRUDResourceHandler>;

class ExternalCRUDResourceHandler;
using ExternalCRUDResourceHandlerUPtr = std::unique_ptr<ExternalCRUDResourceHandler>;

class WebSockAPIModule;
class WSServer;

struct ClientMessage;
struct ServerMessage;
struct RequestContext;
using FiberHandlerT =
    std::function<boost::optional<nlohmann::json>(const RequestContext &)>;

class Facade;
using FacadeUPtr = std::unique_ptr<Facade>;

class Service;
}
}
}
