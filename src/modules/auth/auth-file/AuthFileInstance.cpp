#include "AuthFileInstance.hpp"
#include "tools/log.hpp"

AuthFileInstance::AuthFileInstance(zmqpp::context &ctx,
        std::string const &auth_ctx_name,
        std::string const &auth_target_name,
        std::string const &valid_input_file) :
        bus_push_(ctx, zmqpp::socket_type::push),
        bus_sub_(ctx, zmqpp::socket_type::sub),
        name_(auth_ctx_name),
        file_stream_(valid_input_file)
{
    bus_push_.connect("inproc://zmq-bus-pull");
    bus_sub_.connect("inproc://zmq-bus-pub");

    INFO("Auth instance (" << auth_ctx_name << ") subscribe to " << auth_target_name);
    bus_sub_.subscribe("S_" + auth_target_name);

    if (!file_stream_.good() || !file_stream_.is_open())
        throw std::runtime_error("Cannot access file containing credentials");
}

AuthFileInstance::~AuthFileInstance()
{
    LOG() << "auth contenxt instance down";
}

void AuthFileInstance::handle_bus_msg()
{
    zmqpp::message auth_msg;
    zmqpp::message auth_result_msg;
    std::string topic;
    std::string auth_data;

    bus_sub_.receive(auth_msg);
    assert(auth_msg.parts() == 2);
    auth_msg >> topic;
    auth_msg >> auth_data;

    bool success = false;
    while (!file_stream_.eof())
    {
        std::string line;
        std::getline(file_stream_, line);

        if (line == auth_data)
        {
            LOG() << "Auth context (" << name_ << ") receive valid auth data: " << auth_data;
            success = true;
            break;
        }
    }

    file_stream_.clear();
    file_stream_.seekg(std::ios_base::beg);

    auth_result_msg << ("S_" + name_);

    if (success)
        auth_result_msg << "GRANTED";
    else
        auth_result_msg << "DENIED";
    auth_result_msg << auth_data;
    bus_push_.send(auth_result_msg);
}

zmqpp::socket &AuthFileInstance::bus_sub()
{
    return bus_sub_;
}
