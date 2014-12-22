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

#include <memory>
#include <zmqpp/message.hpp>
#include <boost/property_tree/ptree.hpp>
#include <tools/log.hpp>
#include <zmqpp/context.hpp>
#include "zmqpp/zmqpp.hpp"

/**
* Send the request to the target and handle the response.
* Return false if no response in 1s.
*/
bool send_request(std::shared_ptr<zmqpp::socket> target, const std::string &cmd1)
{
    target->send(cmd1);

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
/**
* pipe is pipe back to module manager.
* this function is called in its own thread.
*
* do signaling when ready
*/
extern "C" __attribute__((visibility("default"))) bool start_module(zmqpp::socket *pipe,
        boost::property_tree::ptree /* cfg */,
        zmqpp::context &zmq_ctx)
{
    INFO("Module stdin-controller is ready. Signaling parent.");
    pipe->send(zmqpp::signal::ok);

    // map a device name or anything that can be a target for a command to socket that are connected to it.
    std::map<std::string, std::shared_ptr<zmqpp::socket>> endpoints_;

    zmqpp::poller p;

    p.add(*pipe, zmqpp::poller::poll_in);
    p.add(0, zmqpp::poller::poll_in);
    while (true)
    {
        p.poll(-1);

        if (p.has_input(*pipe))
        {
            break;
        }

        if (p.has_input(0))
        {

            if (std::cin.fail())
                WARN("FAILED");
            //          std::string txt;
            // fixme bug if input is > buffer
            std::array<char, 4096> txt;
            std::cin.getline(&txt[0], 4096);
            std::string tmp(&txt[0]);
            std::stringstream ss(tmp);

            std::string target;
            std::string cmd1;

            ss >> target;
            ss >> cmd1;
            if (!target.empty())
            {
                if (endpoints_.count(target) == 0)
                {
                    endpoints_[target] = std::shared_ptr<zmqpp::socket>(new zmqpp::socket(zmq_ctx, zmqpp::socket_type::req));
                    endpoints_[target]->connect("inproc://" + target);
                }

                DEBUG("Read {" << std::string(&txt[0]) << "}, target = " << target);

                if (!send_request(endpoints_[target], cmd1))
                {
                    endpoints_.erase(target);
                }
            }
        }
        else if (p.has_error(0))
        {
            INFO("stop polling on stdin");
            p.remove(0);
            continue;
        }

    }

    INFO("Module stdin-controller shutting down");
    return true;
}
