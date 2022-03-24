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

#include "StdinControllerModule.hpp"

using namespace Leosac::Module;

StdinControllerModule::StdinControllerModule(zmqpp::context &ctx,
                                             zmqpp::socket *pipe,
                                             boost::property_tree::ptree const &cfg,
                                             CoreUtilsPtr utils)
    : BaseModule(ctx, pipe, cfg, utils)
{
    reactor_.add(0, std::bind(&StdinControllerModule::handleStdin, this));
}

void StdinControllerModule::handleStdin()
{
    std::array<char, 4096> txt;
    std::cin.getline(&txt[0], 4096);
    std::string tmp(&txt[0]);
    std::stringstream ss(tmp);

    std::string target;
    std::string tmp2;
    std::vector<std::string> cmds;

    ss >> target;
    while (ss >> tmp2)
        cmds.push_back(tmp2);
    if (!target.empty())
    {
        if (endpoints_.count(target) == 0)
        {
            endpoints_[target] = std::shared_ptr<zmqpp::socket>(
                new zmqpp::socket(ctx_, zmqpp::socket_type::req));
            endpoints_[target]->connect("inproc://" + target);
        }

        DEBUG("Read {" << std::string(&txt[0]) << "}, target = " << target);

        if (!send_request(endpoints_[target], cmds))
        {
            endpoints_.erase(target);
        }
    }
}

static bool is_number(const std::string &s)
{
    return !s.empty() && std::find_if(s.begin(), s.end(), [](char c) {
                             return !std::isdigit(c);
                         }) == s.end();
}

bool StdinControllerModule::send_request(std::shared_ptr<zmqpp::socket> target,
                                         const std::vector<std::string> &cmds)
{
    zmqpp::message msg;

    for (const auto &str : cmds)
    {
        if (is_number(str))
        {
            int64_t nbr;
            std::stringstream ss(str);
            ss >> nbr;
            msg << nbr;
        }
        else
        {
            msg << str;
        }
    }
    target->send(msg);

    zmqpp::poller p;

    p.add(*target.get(), zmqpp::poller::poll_in);
    if (!p.poll(1000))
    {
        WARN("No response from target (" << target << ")");
        return false;
    }
    // handle response
    zmqpp::message_t m;
    target->receive(m);

    std::string rep;
    m >> rep;
    INFO("response = " << rep);
    return true;
}
