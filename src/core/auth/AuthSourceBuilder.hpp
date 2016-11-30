/*
    Copyright (C) 2014-2016 Leosac

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

#include "core/auth/Interfaces/IAuthenticationSource.hpp"
#include <zmqpp/message.hpp>

#pragma once

namespace Leosac
{
namespace Auth
{
/**
* This class is some kind of factory to create IAuthenticationSource object
* from a zmqpp::message sent by a AuthSource module (eg Wiegand).
*
* The AuthenticationSource will not contain all data. It still needs to be mapped
* to a user/access profile.
*/
class AuthSourceBuilder
{
  public:
    explicit AuthSourceBuilder()                 = default;
    virtual ~AuthSourceBuilder()                 = default;
    AuthSourceBuilder(const AuthSourceBuilder &) = delete;
    AuthSourceBuilder(AuthSourceBuilder &&)      = delete;
    AuthSourceBuilder &operator=(const AuthSourceBuilder &) = delete;
    AuthSourceBuilder &operator=(AuthSourceBuilder &&) = delete;

    /**
    * Create an AuthenticationSource object from a message.
    *
    * @param msg message from auth source module, CANNOT BE NULL.
    */
    virtual IAuthenticationSourcePtr create(zmqpp::message *msg);

    /**
    * Extract the source name from the frame.
    * @param output shall not be null.
    * @return true if it succeed, false otherwise.
    * @see @ref auth_specc
    */
    bool extract_source_name(const std::string &input, std::string *output) const;

  protected:
    /**
    * Create an auth source from SIMPLE_WIEGAND data type.
    * @param msg the message first frame shall be wiegand data (ie previous should be
    * pop'd).
    */
    IAuthenticationSourcePtr create_simple_wiegand(const std::string &name,
                                                   zmqpp::message *msg);

    /**
    * Create an auth source from WIEGAND_PIN data type.
    */
    IAuthenticationSourcePtr create_wiegand_pin(const std::string &name,
                                                zmqpp::message *msg);

    /**
    * Create an auth source from a WiegandCard and PIN Code.
    */
    IAuthenticationSourcePtr create_wiegand_card_pin(const std::string &name,
                                                     zmqpp::message *msg);
};
}
}
