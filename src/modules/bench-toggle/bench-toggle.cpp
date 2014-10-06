#include <memory>
#include <zmqpp/message.hpp>
#include <boost/property_tree/ptree.hpp>
#include <tools/log.hpp>
#include <zmqpp/context.hpp>
#include "zmqpp/zmqpp.hpp"
#include "hardware/device/FGPIO.hpp"

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
        LOG() << "No response from target (" << target << ")";
        return false;
        }
// handle response
    zmqpp::message_t m;
    target->receive(m);

    std::string rep;
    m >> rep;
    LOG() << "response = " << rep;
    return true;
    }

/**
* pipe is pipe back to module manager.
* this function is called in its own thread.
*
* do signaling when ready
*/
extern "C" __attribute__((visibility("default"))) bool start_module(zmqpp::socket *pipe,
        boost::property_tree::ptree cfg,
zmqpp::context &zmq_ctx)
    {
    // assume custom module startup code.
    // when reeady, signal parent
    std::cout << "Init ok (myname = " << cfg.get_child("name").data() << "... sending OK" << std::endl;
    std::string endpoint_to_bench = cfg.get_child("module_config").get_child("target").data();
    int itr = cfg.get_child("module_config").get<int>("iterations");
    int wait_for = cfg.get_child("module_config").get<int>("pause");
    pipe->send(zmqpp::signal::ok);

    // fixme since module initializtion is not defined we need to wait.
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    std::shared_ptr<zmqpp::socket> sock(new zmqpp::socket(zmq_ctx, zmqpp::socket_type::req));

    sock->connect("inproc://" + endpoint_to_bench);
    LOG() << "should take about " << itr * wait_for << "ms to run";
    std::chrono::system_clock::time_point clock = std::chrono::system_clock::now();
        FGPIO my_gpio(zmq_ctx, endpoint_to_bench);
    for (int i = 0; i < itr; i++)
        {
       // send_request(sock, "TOGGLE");
            my_gpio.toggle();
        std::this_thread::sleep_for(std::chrono::milliseconds(wait_for));
        }
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - clock);
    LOG() << "TOOK " << std::to_string(elapsed.count()) << "ms";
       std::cout << "module bench toggle shutting down" << std::endl;
    return true;
    }
