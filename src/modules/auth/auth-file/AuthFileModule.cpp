#include "AuthFileModule.hpp"


AuthFileModule::AuthFileModule(zmqpp::context &ctx,
            zmqpp::socket *pipe,
            const boost::property_tree::ptree &cfg) :
        ctx_(ctx),
        pipe_(*pipe),
config_(cfg),
is_running_(true)
{
    process_config();

    for (auto authenticator : authenticators_)
    {

    }
    reactor_.add(pipe_, std::bind(&AuthFileModule::handle_pipe, this));
}
