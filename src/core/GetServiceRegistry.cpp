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

#include "core/GetServiceRegistry.hpp"
#include "core/kernel.hpp"

namespace Leosac
{
ServiceRegistry &get_service_registry()
{
    // Normal mode. Kernel instance should have be available
    // when leosac runs.
    if (Kernel::instance_)
        return Kernel::instance_->service_registry();

    // This is a case we can hit when running unit tests.
    // So we just return a dummy and empty registry.
    static ServiceRegistry dummy_registry;
    return dummy_registry;
}
}
