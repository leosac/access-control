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

#include "FetchRemoteConfig.hpp"
#include <tools/log.hpp>

using namespace Leosac;
using namespace Leosac::Tasks;

FetchRemoteConfig::FetchRemoteConfig(const std::string &endpoint,
                                     const std::string &pubkey)
    : ctx_()
    , collector_(ctx_, endpoint, pubkey)
{
    INFO("Creating FetchRemoteConfig task. Guid = " << get_guid());
}

bool FetchRemoteConfig::do_run()
{
    return collector_.fetch_config(nullptr);
}

const RemoteConfigCollector &FetchRemoteConfig::collector() const
{
    return collector_;
}
