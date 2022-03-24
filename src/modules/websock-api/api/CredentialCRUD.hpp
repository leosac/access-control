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

#include "api/CRUDResourceHandler.hpp"
#include "core/auth/AuthFwd.hpp"

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
/**
 * CRUD Handler for credentials.
 *
 * This handler is designed to be able to manage various type of
 * credentials.
 *
 * @see See the *_impl() method for each requests parameters/response.
 */
class CredentialCRUD : public CRUDResourceHandler
{
  private:
    CredentialCRUD(RequestContext ctx);

  public:
    static CRUDResourceHandlerUPtr instanciate(RequestContext);

  private:
    virtual std::vector<ActionActionParam>
    required_permission(Verb verb, const json &req) const override;

    /**
     * Create a new credential.
     *
     * Request:
     *     + `credential-type`: The type of the credential we wish to create.
     *        Can be `rfid-card`.
     *     + `attributes`: Dictionary of attributes for the credential.
     *
     * @param req
     * @return
     */
    virtual boost::optional<json> create_impl(const json &req) override;

    virtual boost::optional<json> read_impl(const json &req) override;

    virtual boost::optional<json> update_impl(const json &req) override;

    virtual boost::optional<json> delete_impl(const json &req) override;
};
}
}
}
