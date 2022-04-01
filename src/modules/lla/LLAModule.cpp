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

#include "LLAModule.hpp"
#include "core/kernel.hpp"
#include "modules/lla/Worker.hpp"
#include "tools/enforce.hpp"
#include "tools/log.hpp"
#include "tools/timeout.hpp"
#include <boost/iterator/transform_iterator.hpp>
#include <boost/property_tree/ptree.hpp>
#include <memory>
#include <zmqpp/context.hpp>
#include <zmqpp/message.hpp>


using namespace Leosac::Module::LLA;

LLAModule::LLAModule(zmqpp::context &ctx, zmqpp::socket *module_manager_pipe,
                     const boost::property_tree::ptree &config, CoreUtilsPtr utils)
    : BaseModule(ctx, module_manager_pipe, config, utils)
{
    process_config();
}

void LLAModule::run()
{
    std::vector<std::unique_ptr<Worker>> workers_;
    for (const auto &wcfg : worker_configs_)
    {
        workers_.push_back(std::make_unique<Worker>(wcfg, ctx_));
    }
    BaseModule::run();
    for (auto &worker_uptr : workers_)
    {
        worker_uptr->stop();
    }
}

void LLAModule::process_config()
{
    boost::property_tree::ptree module_config = config_.get_child("module_config");
    Tools::XmlNodeNameEnforcer xnne(""); // fixme need proper path to config file.

    for (auto &node : module_config.get_child("sources"))
    {
        xnne("source", node.first);
        boost::property_tree::ptree reader_cfg = node.second;

        WorkerConfiguration wcfg;
        wcfg.source_name   = reader_cfg.get<std::string>("name");
        wcfg.polling_time = reader_cfg.get<int>("polling_time", 1000);
        wcfg.readerformat.unSerialize(reader_cfg);
        worker_configs_.push_back(wcfg);
    }
}
