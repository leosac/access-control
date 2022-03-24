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

#include "ExceptionConverter.hpp"
#include "Exceptions.hpp"
#include "exception/EntityNotFound.hpp"
#include "exception/InvalidArgument.hpp"
#include "exception/ModelException.hpp"
#include "exception/PermissionDenied.hpp"
#include "tools/log.hpp"
#include <boost/type_index.hpp>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

ServerMessage ExceptionConverter::convert_merge(const std::exception_ptr &ptr,
                                                const ServerMessage &msg)
{
    auto converted_msg = convert_impl(ptr);
    converted_msg.uuid = msg.uuid;
    converted_msg.type = msg.type;

    return converted_msg;
}

/**
 * Pretty exception name from eptr or empty string if no exception.
 */
static std::string exception_name(const std::exception_ptr &ptr)
{
    try
    {
        if (ptr)
            std::rethrow_exception(ptr);
        return "";
    }
    catch (const std::exception &e)
    {
        auto type_index = boost::typeindex::type_id_runtime(e);
        return type_index.pretty_name();
    }
}

ServerMessage ExceptionConverter::convert_impl(const std::exception_ptr &ptr)
{
    ServerMessage response;
    std::string ename = exception_name(ptr);
    try
    {
        if (ptr)
            std::rethrow_exception(ptr);
        return response;
    }
    catch (const InvalidCall &e)
    {
        response.status_code   = APIStatusCode::INVALID_CALL;
        response.status_string = e.what();
    }
    catch (const PermissionDenied &e)
    {
        response.status_code   = APIStatusCode::PERMISSION_DENIED;
        response.status_string = e.what();
    }
    catch (const MalformedMessage &e)
    {
        WARN("ST: " << e.trace().str());
        response.status_code   = APIStatusCode::MALFORMED;
        response.status_string = e.what();
    }
    catch (const SessionAborted &e)
    {
        response.status_code   = APIStatusCode::SESSION_ABORTED;
        response.status_string = e.what();
    }
    catch (const EntityNotFound &e)
    {
        response.status_code            = APIStatusCode::ENTITY_NOT_FOUND;
        response.status_string          = e.what();
        response.content["entity_id"]   = e.entity_id();
        response.content["entity_type"] = e.entity_type();
    }
    catch (const ModelException &e)
    {
        response.status_code       = APIStatusCode::MODEL_EXCEPTION;
        response.status_string     = e.what();
        response.content["errors"] = e.json_errors();
    }
    catch (const InvalidArgument &e)
    {
        response.status_code   = APIStatusCode::INVALID_ARGUMENT;
        response.status_string = e.what();
    }
    catch (const LEOSACException &e)
    {
        WARN("Leosac specific exception has been caught: " << e.what() << std::endl
                                                           << e.trace().str());
        response.status_code   = APIStatusCode::GENERAL_FAILURE;
        response.status_string = e.what(); // todo Maybe remove in production.
    }
    catch (const odb::exception &e)
    {
        ERROR("Database Error: " << e.what());
        response.status_code   = APIStatusCode::DATABASE_ERROR;
        response.status_string = "Database Error: " + std::string(e.what());
    }
    catch (const std::exception &e)
    {
        WARN("Exception when processing request: " << e.what());
        response.status_code   = APIStatusCode::GENERAL_FAILURE;
        response.status_string = e.what();
    }
    // If we're here, we had an exception. Prepend the status_string
    // with the type of the exception.
    response.status_string =
        BUILD_STR("[" << ename << "]: " << response.status_string);
    return response;
}
