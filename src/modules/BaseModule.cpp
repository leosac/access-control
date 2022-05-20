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

#include "BaseModule.hpp"
#include "core/CoreUtils.hpp"
#include "core/config/ConfigManager.hpp"
#include "tools/XmlPropertyTree.hpp"
#include "tools/log.hpp"
#include <boost/archive/text_oarchive.hpp>
#include <boost/property_tree/ptree_serialization.hpp>
#include <signal.h>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Tools;

BaseModule::BaseModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                       boost::property_tree::ptree const &cfg, CoreUtilsPtr utils)
    : ctx_(ctx)
    , pipe_(*pipe)
    , config_(cfg)
    , utils_(utils)
    , is_running_(true)
    , control_(ctx, zmqpp::socket_type::rep)
{
    name_ = cfg.get<std::string>("name");
    control_.bind("inproc://module-" + name_);

    reactor_.add(control_, std::bind(&BaseModule::handle_control, this));
    reactor_.add(pipe_, std::bind(&BaseModule::handle_pipe, this));
}

CoreUtilsPtr BaseModule::utils() const
{
  return utils_;
}

void BaseModule::run()
{
    while (is_running_)
    {
        reactor_.poll();
    }
}

void BaseModule::handle_pipe()
{
    zmqpp::message msg;
    zmqpp::signal sig;
    pipe_.receive(msg);

    assert(msg.is_signal());
    msg >> sig;
    if (sig == zmqpp::signal::stop)
        is_running_ = false;
    else
    {
        ERROR(
            "Module receive a message on its pipe that wasn't a signal. Aborting.");
        assert(0);
        throw std::runtime_error(
            "Module receive a message on its pipe that wasn't a signal. Aborting.");
    }
}

void BaseModule::dump_config(ConfigManager::ConfigFormat fmt,
                             zmqpp::message *out_msg) const
{
    assert(out_msg);
    if (fmt == ConfigManager::ConfigFormat::BOOST_ARCHIVE)
    {
        std::ostringstream oss;
        boost::archive::text_oarchive archive(oss);
        boost::property_tree::save(archive, config_, 1);
        out_msg->add(oss.str());
    }
    else
    {
        out_msg->add(propertyTreeToXml(config_));
    }
    try
    {
        dump_additional_config(out_msg);
    }
    catch (std::exception &e)
    {
        ERROR("Problem while dumping config: " << e.what());
    }
}

void BaseModule::handle_control()
{
    zmqpp::message msg;
    std::string frame1;

    control_.receive(msg);
    msg >> frame1;
    if (frame1 == "DUMP_CONFIG")
    {
        zmqpp::message response;

        assert(msg.remaining() == 1);
        ConfigManager::ConfigFormat format;
        msg >> format;
        DEBUG("Module " << name_ << " is dumping config!");
        dump_config(format, &response);
        control_.send(response);
    }
    else
    {
        ERROR("Module received invalid request (" << frame1 << "). Aborting.");
        assert(0);
        throw std::runtime_error("Invalid request for module.");
    }
}

void BaseModule::dump_additional_config(zmqpp::message *) const
{
}

void BaseModule::config_check(const std::string &obj_name)
{
    if (utils_->config_checker().has_object(obj_name))
        return;

    std::string prefix = "Configuration Error (module " + name_ + ") ";
    ERROR(prefix << "Object " << obj_name << " cannot be found.");

    if (utils_->is_strict())
        raise(SIGABRT); // BOOM
}

void BaseModule::config_check(const std::string &obj_name,
                              Leosac::Hardware::DeviceClass type)
{
    bool res = utils_->config_checker().has_object(obj_name, type);

    if (res)
        return;

    std::string prefix = "Configuration Error (module " + name_ + ") ";

    switch (type)
    {
    case Leosac::Hardware::DeviceClass::GPIO:
        ERROR(prefix << "GPIO " << obj_name << " doesn't exist.");
        break;
    case Leosac::Hardware::DeviceClass::LED:
        ERROR(prefix << "LED " << obj_name << " doesn't exist.");
        break;
    case Leosac::Hardware::DeviceClass::BUZZER:
        ERROR(prefix << "BUZZER " << obj_name << " doesn't exists.");
        break;
    case Leosac::Hardware::DeviceClass::RFID_READER:
        ERROR(prefix << "READER " << obj_name << " doesn't exists.");
        break;
    case Leosac::Hardware::DeviceClass::EXTERNAL_SERVER:
        ERROR(prefix << "EXTERNAL SERVER " << obj_name << " doesn't exists.");
        break;
    default:
        ASSERT_LOG(false, prefix << "Missing case in switch: value "
                                 << static_cast<int>(type) << " Need code fix.");
        raise(SIGABRT);
        break;
    }
    if (utils_->is_strict())
        raise(SIGABRT); // lets suicide.
}
