#include "RplethModule.hpp"

RplethModule::RplethModule(zmqpp::context &ctx,
        zmqpp::socket *pipe,
        const boost::property_tree::ptree &cfg) :
        BaseModule(ctx, pipe, cfg)
{

}
