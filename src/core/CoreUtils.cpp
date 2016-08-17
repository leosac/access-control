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

#include "tools/log.hpp"
#include "tools/db/db_fwd.hpp"
#include "CoreUtils.hpp"
#include "kernel.hpp"

Leosac::CoreUtils::CoreUtils(Leosac::Kernel *kptr,
                             Leosac::SchedulerPtr sched,
                             Leosac::ConfigCheckerPtr cfgcheck,
                             bool strict_mode) :
        kptr_(kptr),
        scheduler_(sched),
        config_checker_(cfgcheck),
        strict_mode_(strict_mode)
{

}

Leosac::CoreUtils::CoreUtils() :
        kptr_(nullptr),
        strict_mode_(false)
{
}

Leosac::Scheduler &Leosac::CoreUtils::scheduler()
{
    ASSERT_LOG(scheduler_, "No scheduler object in CoreUtils.");
    return *scheduler_;
}

Leosac::ConfigChecker &Leosac::CoreUtils::config_checker()
{
    ASSERT_LOG(config_checker_, "No ConfigChecker object in CoreUtils.");
    return *config_checker_;
}

Leosac::DBPtr Leosac::CoreUtils::database()
{
    ASSERT_LOG(kptr_, "Kernel pointer is NULL in CoreUtils.");
    return kptr_->database();
}

bool Leosac::CoreUtils::is_strict() const
{
    return strict_mode_;
}

Leosac::Kernel &Leosac::CoreUtils::kernel()
{
    ASSERT_LOG(kptr_, "Kernel pointer is NULL in CoreUtils");
    return *kptr_;
}

Leosac::CoreAPI Leosac::CoreUtils::core_api()
{
    return Leosac::CoreAPI(kernel());
}
