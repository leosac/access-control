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

#pragma once

#include "AuthDBFwd.hpp"
#include "AsioModule.hpp"
#include "core/CoreUtils.hpp"
#include <list>
#include <memory>

namespace Leosac
{
namespace Module
{
namespace Auth
{

/**
 * Module that provides authentication using database-stored credentials
 */
class AuthDBModule : public AsioModule
{
    public:
        AuthDBModule(zmqpp::context& ctx, zmqpp::socket *pipe, 
                     const boost::property_tree::ptree &cfg, CoreUtilsPtr utils);
        
        AuthDBModule(const AuthDBModule &) = delete;
        ~AuthDBModule();

        
        /**
         * Ignore less than this number of bits from bus
         */
        int bits_low_threshold_;

        /**
        * Ignore bits higher than this number of bits from bus
        */
        int bits_high_threshold_;
    
    protected:
        void on_service_event(const service_event::Event &event) override;

    private:
        /** 
         * Process the module configuration from the kernel
         */
        void process_config();

        /**
         * Setup authenticator instances
         */
        void setup_authenticators(const std::string &auth_ctx_name, 
                                  const std::list<std::string> &auth_sources_names, 
                                  const std::string &auth_target_name);

        /**
         * List of the created authenticator instances
         */
        std::list<AuthDBInstancePtr> authenticators_;
};
}
}
}