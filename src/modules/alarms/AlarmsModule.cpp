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

#include "modules/alarms/AlarmsModule.hpp"
#include "modules/alarms/AlarmInstance.hpp"
#include "hardware/Alarm_odb.h"
#include "hardware/GPIO_odb.h"
#include "core/CoreUtils.hpp"
#include "core/kernel.hpp"
#include <tools/db/database.hpp>

using namespace Leosac;
using namespace Leosac::Module::Alarms;

AlarmsModule::AlarmsModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                           const boost::property_tree::ptree &cfg,
                           CoreUtilsPtr utils)
    : BaseModule(ctx, pipe, cfg, utils)
{
    process_config();

    for (auto &alarm : alarms_)
    {
      alarm->register_sockets(&reactor_);
    }
}

AlarmsModule::~AlarmsModule()
{
}

void AlarmsModule::process_config()
{
  boost::property_tree::ptree module_config = config_.get_child("module_config");

  if (module_config.get<bool>("use_database", false))
  {
    load_db_config();
  }
  else
  {
    load_xml_config(module_config);
  }
}

void AlarmsModule::load_db_config()
{
    using namespace odb;
    using namespace odb::core;
    auto db = utils_->database();

    // First we load or update database schema if needed.
    schema_version v = db->schema_version("module_alarms");
    schema_version cv(schema_catalog::current_version(*db, "module_alarms"));
    if (v == 0)
    {
        transaction t(db->begin());
        INFO("Attempt to create module_alarms SQL schema.");
        schema_catalog::create_schema(*db, "module_alarms");
        t.commit();
    }
    else if (v < cv)
    {
        INFO("Alarms Module performing database migration. Going from version "
             << v << " to version " << cv);
        transaction t(db->begin());
        schema_catalog::migrate(*db, cv, "module_alarms");
        t.commit();
    }

    // ... then loads servers from database.
    odb::transaction t(db->begin());
    odb::result<Hardware::Alarm> sresult(db->query<Hardware::Alarm>());
    for (const auto &alarm : sresult)
    {
        Hardware::AlarmPtr alarm_ptr =
            db->load<Hardware::Alarm>(alarm.id());
        ASSERT_LOG(alarm_ptr, "Loading from database/cache failed");
        if (alarm_ptr->enabled())
        {
            std::string gpio_name;
            if (alarm_ptr->gpio())
            {
                gpio_name = alarm_ptr->gpio()->name();
            }

            auto alarm = std::make_shared<AlarmInstance>(ctx_, db, alarm_ptr, gpio_name);
            utils_->config_checker().register_object(alarm->name(),
                                                     Leosac::Hardware::DeviceClass::ALARM);
            alarms_.push_back(alarm);
        }
    }
    t.commit();

    INFO("Alarms module using SQL database for configuration.");
}

void AlarmsModule::load_xml_config(
    const boost::property_tree::ptree &module_config)
{
    for (auto &node : module_config.get_child("alarms"))
    {
        auto alarm_config = std::make_shared<Hardware::Alarm>();
        boost::property_tree::ptree xml_alarm_cfg = node.second;

        std::string alarm_name = xml_alarm_cfg.get_child("name").data();
        alarm_config->name(alarm_name);
        alarm_config->severity(static_cast<Hardware::Alarm::AlarmSeverity>(xml_alarm_cfg.get<uint8_t>("severity", 2)));
        std::string gpio_name = xml_alarm_cfg.get<std::string>("gpio", "");
        if (!gpio_name.empty())
        {
          config_check(gpio_name, Leosac::Hardware::DeviceClass::GPIO);
        }

        auto alarm = std::make_shared<AlarmInstance>(ctx_, nullptr, alarm_config, gpio_name);
        utils_->config_checker().register_object(alarm->name(),
                                                 Leosac::Hardware::DeviceClass::ALARM);
        alarms_.push_back(alarm);
    }
}
