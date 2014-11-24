#include "BaseModule.hpp"

using namespace Leosac::Module;

BaseModule::BaseModule(zmqpp::context &ctx,
        zmqpp::socket *pipe,
        boost::property_tree::ptree const &cfg) :
        ctx_(ctx),
        pipe_(*pipe),
        config_(cfg),
        is_running_(true)
{
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
    zmqpp::signal sig;
    pipe_.receive(sig);

    if (sig == zmqpp::signal::stop)
    {
        is_running_ = false;
    }
    else
        assert(0);
}
