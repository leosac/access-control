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

#include "DoormanModule.hpp"
#include "DoormanInstance.hpp"
#include "core/Scheduler.hpp"
#include "core/auth/Auth.hpp"
#include "core/kernel.hpp"
#include "hardware/facades/FAlarm.hpp"
#include "tools/log.hpp"
#include "tools/service/ServiceRegistry.hpp"
#include "tools/db/DBService.hpp"
#include "tools/Schedule.hpp"
#include "tools/ScheduleMapping.hpp"
#include "core/auth/Door.hpp"
#include "core/auth/Door_odb.h"

using namespace Leosac::Module::Doorman;
using namespace Leosac::Auth;

DoormanModule::DoormanModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                             const boost::property_tree::ptree &cfg,
                             CoreUtilsPtr utils)
    : BaseModule(ctx, pipe, cfg, utils)
    , use_db_schedules_(false)
    , db_service_(nullptr)
    
{
    try
    {
        process_config();
    }
    catch (boost::property_tree::ptree_error &e)
    {
        std::throw_with_nested(
            ConfigException("main", "Doorman module configuration is invalid"));
    }

    for (auto &&doorman : doormen_)
    {
        reactor_.add(doorman->bus_sub(),
                     std::bind(&DoormanInstance::handle_bus_msg, doorman));

        for (auto &&door : doorman->doors())
        {
          reactor_.add(door->bus_sub(),
                       std::bind(&DoormanDoor::handle_bus_msg, door));
        }
    }
}

void DoormanModule::process_config()
{
    boost::property_tree::ptree module_config = config_.get_child("module_config");

    use_db_schedules_ = module_config.get<bool>("use_db_schedules", false);
    set_db_service();

    auto doors_cfg = module_config.get_child_optional("doors");
    if (doors_cfg)
        process_doors_config(*doors_cfg);
    if (use_db_schedules_)
        process_db_schedules();

    for (const auto &node : module_config.get_child("instances"))
    {
        // one doorman instance
        boost::property_tree::ptree cfg_doorman = node.second;

        std::vector<std::string> auth_ctx_names;
        std::vector<DoormanAction> actions;
        std::string doorman_name = cfg_doorman.get_child("name").data();

        for (const auto &auth_contexts_node : cfg_doorman.get_child("auth_contexts"))
        {
            // each auth context we listen to
            auth_ctx_names.push_back(
                auth_contexts_node.second.get<std::string>("name"));
        }

        for (const auto &action_node : cfg_doorman.get_child("actions"))
        {
            // every action we take
            boost::property_tree::ptree cfg_action = action_node.second;
            DoormanAction a;
            std::string on_status;

            on_status = cfg_action.get<std::string>("on");
            a.on_     = (on_status == "GRANTED" ? AccessStatus::GRANTED
                                            : AccessStatus::DENIED);
            a.target_ = cfg_action.get<std::string>("target");
            config_check(a.target_);

            for (auto &cmd_node : cfg_action.get_child("cmd"))
            {
                // each frame in command
                // fixme ORDER
                // fixme ONLY ONE FRAME
                a.cmd_.push_back(cmd_node.second.data());
            }
            actions.push_back(a);
        }

        INFO("Creating Doorman instance " << doorman_name);
        auto instance = std::make_shared<DoormanInstance>(
            *this, ctx_, doorman_name, auth_ctx_names, actions);
        doormen_.push_back(instance);
    }
}

void DoormanModule::set_db_service() {
    if (use_db_schedules_) {
        db_service_ = utils_->service_registry().get_service<DBService>();
        if (!db_service_) {
            WARN("Database schedules requested but DBService not available. Falling back to config schedules.");
            use_db_schedules_ = false;
        } else {
            INFO("Using database schedules for doorman module");
        }
    }
}

void DoormanModule::run()
{
    while (is_running_)
    {
        update();
        reactor_.poll(2000);
    }
}

void DoormanModule::process_doors_config(
    const boost::property_tree::ptree &doors_cfg)
{
    DEBUG("Processing doors config");
    for (const auto &door_cfg : doors_cfg)
    {
        std::string name = door_cfg.second.get<std::string>("name");
        std::string gpio = door_cfg.second.get<std::string>("gpio");
        const auto &open_schedule =
            door_cfg.second.get_child_optional("on.schedules");
        const auto &close_schedule =
            door_cfg.second.get_child_optional("off.schedules");

        AuthTargetPtr door(new AuthTarget(name));
        door->gpio(
            std::unique_ptr<Hardware::FGPIO>(new Hardware::FGPIO(ctx_, gpio)));

        if (!use_db_schedules_ && open_schedule)
        {
            Tools::XmlScheduleLoader xml_sched;
            xml_sched.load(*open_schedule);
            for (const auto &map_entry : xml_sched.schedules())
            {
                door->add_always_open_sched(map_entry.second);
            }
        }
        if (close_schedule)
        {
            Tools::XmlScheduleLoader xml_sched;
            xml_sched.load(*close_schedule);
            for (const auto &map_entry : xml_sched.schedules())
            {
                door->add_always_close_sched(map_entry.second);
            }
        }

        const auto &exitreq =
            door_cfg.second.get_child_optional("exitreq");
        if (exitreq)
        {
          std::string exitreq_gpio = exitreq.get().get<std::string>("gpio");
          door->exitreq_gpio(
              std::unique_ptr<Hardware::FGPIO>(new Hardware::FGPIO(ctx_, exitreq_gpio)));
          door->exitreq_duration(std::chrono::milliseconds(exitreq.get().get<uint16_t>("duration")));
        }

        const auto &contact =
            door_cfg.second.get_child_optional("contact");
        if (contact)
        {
          std::string contact_gpio = contact.get().get<std::string>("gpio");
          door->contact_gpio(
              std::unique_ptr<Hardware::FGPIO>(new Hardware::FGPIO(ctx_, contact_gpio)));
          door->contact_duration(std::chrono::milliseconds(contact.get().get<uint16_t>("duration")));
        }

        const auto &alarm = door_cfg.second.get<std::string>("alarm", "");
        if (!alarm.empty())
        {
          door->alarm(std::unique_ptr<Hardware::FAlarm>(new Hardware::FAlarm(ctx_, alarm)));
        }
        doors_.push_back(door);
    }
}

void DoormanModule::process_db_schedules() {
    try {
        auto db = db_service_->db();
        odb::transaction t(db->begin());
        odb::result<Tools::Schedule> schedules = db->query<Tools::Schedule>();
        std::map<std::string, std::vector<Tools::SingleTimeFrame>> door_open_timeframes;

        clear_door_schedules();

        add_open_door_schedules(schedules);

        t.commit();
    } catch (const std::exception &e) {
        ERROR("Failed to process database schedules: " << e.what());
        use_db_schedules_ = false;
    }
}

void DoormanModule::add_open_door_schedules(odb::result<Tools::Schedule> &schedules) {
    for (const auto &schedule : schedules) {
        for (const auto &mapping : schedule.mapping()) {
            if (is_door_schedule(mapping)) {
                for (const auto &lazy_door : mapping->doors()) {
                    auto door_ptr = lazy_door.load();
                    if (door_ptr) {
                        std::string door_name = door_ptr->alias();
                        for (auto &door : doors_) {
                            if (door->name() == door_name) {
                                auto schedule_copy = std::make_shared<Tools::Schedule>(schedule);
                                door->add_always_open_sched(schedule_copy);
                            }
                        }
                    }
                }
            }
        }
    }
}

void DoormanModule::clear_door_schedules() {
    for (auto &door : doors_)
        door->clear_schedules();
}

bool DoormanModule::is_door_schedule(const Tools::ScheduleMappingPtr &mapping) {
    bool has_doors = !mapping->doors().empty();
    bool has_users = !mapping->users().empty();
    bool has_groups = !mapping->groups().empty();
    bool has_credentials = !mapping->credentials().empty();
    bool has_zones = !mapping->zones().empty();
    
    return (has_doors || has_zones) && !has_users && !has_groups && !has_credentials;
}

void DoormanModule::update()
{
  auto now = std::chrono::system_clock::now();

  for (auto &&doorman : doormen_)
  {
    for (auto &&door : doorman->doors())
    {
      auto d = door->door();
      auto alarm = d->alarm();
      if (alarm != nullptr)
      {
        if (door->alarm_forced().empty() && door->contact_triggered())
        {
          auto gpio = d->gpio();
          if ((door->contact_lastupdate() + d->contact_duration()) >= now)
          {
            door->alarm_forced(alarm->raise(Hardware::AlarmType::ALARM_FORCED, "Door forced (opened too long)."));
          }
          else if (gpio && (gpio->lastupdate() + d->contact_duration()) < now)
          {
            door->alarm_forced(alarm->raise(Hardware::AlarmType::ALARM_FORCED, "Door forced (unexpected opening)."));
          }
        }
        else if (!door->alarm_forced().empty() && !door->contact_triggered())
        {
          if (alarm->state(door->alarm_forced()) == Hardware::AlarmState::STATE_RAISED)
          {
            alarm->disarm(door->alarm_forced());
          }
          door->alarm_forced("");
        }
      }
      d->resetToExpectedState(now);
    }
  }
}

std::vector<AuthTargetPtr> const &DoormanModule::doors() const
{
    return doors_;
}
