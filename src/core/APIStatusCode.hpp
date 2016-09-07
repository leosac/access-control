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

namespace Leosac
{
/**
 * Those are the Leosac API status code.
 *
 * They currently apply only to the WebSocket API.
 *
 * @note This enum is provided by core because the audit system relies on it.
 */
enum class APIStatusCode
{
    /**
     * Indicate that the request's processing went well.
     */
    SUCCESS = 0x00,

    /**
     * A failure for an unknown reason.
     */
    GENERAL_FAILURE = 0x01,

    /**
     * The websocket connection is not allowed to make the
     * requested API call.
     */
    PERMISSION_DENIED = 0x02,

    /**
     * The websocket connection is rate limited, and it already
     * sent too many packets.
     */
    RATE_LIMITED = 0x03,

    /**
     * The source packet was malformed.
     */
    MALFORMED = 0x04,

    /**
     * The API method (ie, message's type) does not exist.
     */
    INVALID_CALL = 0x05,

    /**
     * The request took too long to process.
     * This is mostly here as a placeholder, as this status_code will mostly
     * be used internaly by the Javascript web app to signal a lack of response.
     */
    TIMEOUT = 0x06,

    /**
     * The session has been aborted.
     * This is likely due to the expiration of the token used to
     * authenticate.
     */
    SESSION_ABORTED = 0x07,

    /**
     * The requested entity cannot be found.
     */
    ENTITY_NOT_FOUND = 0x08,

    /**
     * An internal database operation threw an exception.
     */
    DATABASE_ERROR = 0x09,

    /**
     * Unknown status.
     * Mostly useful as a default value somewhere.
     */
    UNKNOWN = 0x0A
};
}