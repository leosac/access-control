#include <tools/log.hpp>
#include "InstrumentationModule.hpp"

using namespace Leosac::Module::Instrumentation;

InstrumentationModule::InstrumentationModule(zmqpp::context &ctx,
        zmqpp::socket *pipe,
        const boost::property_tree::ptree &cfg) :
        BaseModule(ctx, pipe, cfg),
        controller_(ctx, zmqpp::socket_type::router)
{
    std::string bind_str = "ipc:///tmp/leosac-ipc/" + std::to_string(getpid());
    controller_.bind(bind_str);
    INFO("Binding to: " << bind_str);
    reactor_.add(controller_, std::bind(&InstrumentationModule::handle_command, this));
}

void InstrumentationModule::handle_command()
{
    INFO("COMMAND AVAILABLE");
    zmqpp::message msg;

    controller_.receive(msg);

}
