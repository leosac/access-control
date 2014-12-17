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

#include <boost/archive/text_oarchive.hpp>
#include <boost/property_tree/ptree_serialization.hpp>
#include <tools/log.hpp>
#include "BaseModule.hpp"

using namespace Leosac::Module;

BaseModule::BaseModule(zmqpp::context &ctx,
        zmqpp::socket *pipe,
        boost::property_tree::ptree const &cfg) :
        ctx_(ctx),
        pipe_(*pipe),
        config_(cfg),
        is_running_(true),
        control_(ctx, zmqpp::socket_type::rep)
{
    std::string module_name;
    module_name = cfg.get<std::string>("name");
    DEBUG("MODULE NAME = {" << module_name << "}");
    control_.bind("inproc://module-" + module_name);

    reactor_.add(control_, std::bind(&BaseModule::handle_control, this));
    reactor_.add(pipe_, std::bind(&BaseModule::handle_pipe, this));
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
    std::string frame1;
    pipe_.receive(msg);

    assert(msg.is_signal());
    msg >> sig;
    if (sig == zmqpp::signal::stop)
        is_running_ = false;
    else
        assert(0);

}

std::string BaseModule::dump_config() const
{
    std::ostringstream oss;
    boost::archive::text_oarchive archive(oss);
    boost::property_tree::save(archive, config_, 1);

    return oss.str();
}

void BaseModule::handle_control()
{
    zmqpp::message msg;
    std::string frame1;

    control_.receive(msg);
    msg >> frame1;
    DEBUG("HOHO:" << frame1);
    if (frame1 == "DUMP_CONFIG")
    {
        DEBUG("Dumping config!");
        control_.send(dump_config());
    }
    else
        assert(0);
}
