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

#include "api/search/HardwareSearch.hpp"
#include "Exceptions.hpp"
#include "api/APISession.hpp"
#include "core/GetServiceRegistry.hpp"
#include "hardware/GPIO_odb.h"
#include "tools/db/DBService.hpp"
#include "tools/log.hpp"
#include <hardware/HardwareService.hpp>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

HardwareSearch::HardwareSearch(RequestContext ctx)
    : MethodHandler(ctx)
{
}

MethodHandlerUPtr HardwareSearch::create(RequestContext ctx)
{
    return std::make_unique<HardwareSearch>(ctx);
}

struct HardwareComparator
{
    bool operator()(const Hardware::GPIOPtr &c1, const Hardware::GPIOPtr &c2)
    {
        ASSERT_LOG(c1, "Hardware c1 is null");
        ASSERT_LOG(c2, "Hardware c2 is null");
        ASSERT_LOG(!c1->id().is_nil(), "c1 has no id.");
        ASSERT_LOG(!c2->id().is_nil(), "c2 has no id.");
        return c1->id() < c2->id();
    }
};

json HardwareSearch::process_impl(const json &req)
{
    json rep = json::array();
    DBPtr db = ctx_.dbsrv->db();
    odb::transaction t(db->begin());
    using Query              = odb::query<Hardware::GPIO>;
    std::string partial_name = req.at("partial_name");
    std::set<Hardware::GPIOPtr, HardwareComparator> hardware_devices;

    // We want a case insensitive search. However, there is no portable
    // way to do this. So for now we'll rely on a naive, potentially slow
    // bruteforce-style implementation.
    // todo: fixme

    for (auto i = 0u; i < partial_name.length(); ++i)
    {
        auto partial_name_copy = partial_name;
        partial_name_copy[i]   = std::toupper(partial_name[i]);
        Query q(Query::name.like("%" + partial_name_copy + "%"));
        auto results = db->query(q);
        for (const auto &cred : results)
        {
            // We want shared_ptr to store in set. Call load, this should load from
            // the cache anyway.
            hardware_devices.insert(db->load<Hardware::GPIO>(cred.id()));
        }
    }

    Query q(Query::name.like("%" + partial_name + "%"));
    auto results = db->query(q);
    for (const auto &dev : results)
    {
        // We want shared_ptr to store in set. Call load, this should load from the
        // cache anyway.
        hardware_devices.insert(db->load<Hardware::GPIO>(dev.id()));
    }

    auto hardware_service =
        get_service_registry().get_service<Hardware::HardwareService>();
    ASSERT_LOG(hardware_service, "Failed to retrieve hardware service");
    for (const auto &dev : hardware_devices)
    {
        ASSERT_LOG(dev, "Hardware is null.");
        json result_json = {{"id", dev->id()},
                            {"name", dev->name()},
                            {"device_class", dev->device_class()},
                            {"type", hardware_service->hardware_device_type(*dev)}};
        rep.push_back(result_json);
    }

    return rep;
}

std::vector<ActionActionParam>
HardwareSearch::required_permission(const json &) const
{
    std::vector<ActionActionParam> perm_;
    SecurityContext::ActionParam ap{};

    perm_.push_back({SecurityContext::Action::HARDWARE_GPIO_SEARCH, ap});
    return perm_;
}
