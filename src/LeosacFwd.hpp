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

#include <memory>
#include <vector>

namespace Leosac
{
class Kernel;
class Scheduler;
class ConfigChecker;

using SchedulerPtr     = std::shared_ptr<Scheduler>;
using ConfigCheckerPtr = std::shared_ptr<ConfigChecker>;

class CoreUtils;
using CoreUtilsPtr = std::shared_ptr<CoreUtils>;

using ByteVector = std::vector<uint8_t>;

namespace Tasks
{
class Task;
using TaskPtr = std::shared_ptr<Task>;

class SyncConfig;
using SyncConfigPtr = std::shared_ptr<SyncConfig>;

class FetchRemoteConfig;
using FetchRemoteConfigPtr = std::shared_ptr<FetchRemoteConfig>;
}
}
