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
    std::string cmd1;

    ss >> target;
    ss >> cmd1;
    if (!target.empty())
    {
        if (endpoints_.count(target) == 0)
        {
            endpoints_[target] = std::shared_ptr<zmqpp::socket>(
                    new zmqpp::socket(ctx_, zmqpp::socket_type::req));
            endpoints_[target]->connect("inproc://" + target);
        }

        DEBUG("Read {" << std::string(&txt[0]) << "}, target = " << target);

        if (!send_request(endpoints_[target], cmd1))
        {
            endpoints_.erase(target);
        }
    }
}

bool StdinControllerModule::send_request(std::shared_ptr<zmqpp::socket> target,
                                         const std::string &cmd1)
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
