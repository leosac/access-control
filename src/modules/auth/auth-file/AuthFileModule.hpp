/*
    Copyright (C) 2014-2015 Islog

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

#include "AuthFileInstance.hpp"
#include <boost/property_tree/ptree.hpp>
#include <modules/BaseModule.hpp>
#include <vector>
#include <zmqpp/zmqpp.hpp>

namespace Leosac
{
namespace Module
{
namespace Auth
{
class AuthFileInstance;

/**
* This implements a authentication module that use files to store access permissions.
*
* This module conforms to @ref auth_specc.
* @see @ref mod_auth_file_user_config for end user doc
*/
class AuthFileModule : public BaseModule
{
  public:
    AuthFileModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                   const boost::property_tree::ptree &cfg, CoreUtilsPtr utils);

    AuthFileModule(const AuthFileModule &) = delete;

    ~AuthFileModule();

  protected:
    /**
    * We have one config file per authenticator object.
    */
    virtual void dump_additional_config(zmqpp::message *out) const override;

  private:
    /**
    * Processing the configuration tree, spawning AuthFileInstance object as
    * described in the
    * configuration file.
    */
    void process_config();

    /**
    * Authenticator instance.
    */
    std::vector<AuthFileInstancePtr> authenticators_;
};
}
}
}
