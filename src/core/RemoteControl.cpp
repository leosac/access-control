#include <tools/log.hpp>
#include <zmqpp/curve.hpp>
#include "RemoteControl.hpp"
#include "kernel.hpp"
#include <boost/regex.hpp>
#include <cassert>

using namespace Leosac;

RemoteControl::RemoteControl(zmqpp::context &ctx,
        Kernel &kernel,
        const boost::property_tree::ptree &cfg) :
        kernel_(kernel),
        socket_(ctx, zmqpp::socket_type::router),
        auth_(ctx),
        context_(ctx)
{
    auth_.set_verbose(true);
    auth_.configure_curve("CURVE_ALLOW_ANY");
    process_config(cfg);
}

void RemoteControl::process_config(const boost::property_tree::ptree &cfg)
{
    int port = cfg.get<int>("port");

    secret_key_ = cfg.get<std::string>("secret_key");
    public_key_ = cfg.get<std::string>("public_key");

    INFO("Binding RemoteControl socket to port " << port);
    INFO("Use private key {" << secret_key_ << "} and public key {" << public_key_ << "}");

    socket_.set(zmqpp::socket_option::curve_server, true);
    socket_.set(zmqpp::socket_option::curve_secret_key, secret_key_);
    socket_.set(zmqpp::socket_option::curve_public_key, public_key_);
    socket_.bind("tcp://*:" + std::to_string(port));
}

void RemoteControl::handle_msg()
{
    zmqpp::message msg;
    zmqpp::message rep;
    std::string source;
    std::string frame1;
    socket_.receive(msg);

    assert(msg.parts() > 1);
    DEBUG("PARTS = " << msg.parts());
    msg >> source;
    if (std::find(test_.begin(), test_.end(), source) != test_.end())
        DEBUG("FOUND !!!");
    else
        test_.push_back(source);
    rep << source;
    std::stringstream ss;
    for (auto c : source)
        ss << std::hex << (int) c;
    INFO("(" << test_.size() << ") New message from {" << ss.str() << "}");

    msg >> frame1;
    DEBUG("Cmd = {" << frame1 << "}");
    if (frame1 == "MODULE_LIST")
        module_list(&rep);
    else if (frame1 == "MODULE_CONFIG")
    {
        if (msg.remaining() >= 1)
        {
            std::string module_name;
            msg >> module_name;
            module_config(module_name, &rep);
        }
        else
        {
            rep << "MALFORMED MESSAGE";
        }
    }
    else if (frame1 == "SYNC_FROM")
    {
        if (msg.remaining() == 1)
        {
            std::string endpoint;
            msg >> endpoint;
            sync_from(endpoint, &rep);
        }
        else
        {
            rep << "MALFORMED MESSAGE (SYNC_FROM)";
        }
    }

    DEBUG("Sending response, " << rep.parts() << " frames");
    socket_.send(rep);
}

void RemoteControl::module_list(zmqpp::message *message_out)
{
    assert(message_out);
    DEBUG("HERE");
    for (const std::string &s : kernel_.module_manager().modules_names())
    {
        DEBUG("Module {" << s << "}");
        *message_out << s;
    }
}

void RemoteControl::module_config(const std::string &module, zmqpp::message *message_out)
{
    assert(message_out);
    // we need to make sure the module's name exist.
    std::vector<std::string> modules_names = kernel_.module_manager().modules_names();
    if (std::find(modules_names.begin(), modules_names.end(), module) != modules_names.end())
    {
        zmqpp::socket sock(context_, zmqpp::socket_type::req);
        std::string serialized_cfg;
        DEBUG("FOUND " << module);
        sock.connect("inproc://module-" + module);
        bool ret = sock.send(zmqpp::message() << "DUMP_CONFIG" << uint8_t('1'));
        assert(ret);
        DEBUG("HERE");
        sock.receive(serialized_cfg);
        *message_out << "OK";
        *message_out << module;
        *message_out << serialized_cfg;
        return;
    }
    else
    {
        // if module with this name is not found
        *message_out << "KO";
    }
}

static bool validate_endpoint(const std::string &endpoint)
{
    static const boost::regex r_endpoint("tcp://((\\d{1,3}\\.){3}\\d{1,3}):\\d+");
    return regex_match(endpoint, r_endpoint);
}

void RemoteControl::sync_from(const std::string &endpoint, zmqpp::message *message_out)
{
    assert(message_out);
    zmqpp::socket sock(context_, zmqpp::socket_type::dealer);

    if (validate_endpoint(endpoint))
    {
        sock.connect(endpoint);

        INFO("Remote Control module entering blocking mode...");

        gather_remote_config(sock);


        kernel_.module_manager().stopModule("WIEGAND_READER");
        kernel_.module_manager().stopModule("DOORMAN");
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));


                boost::property_tree::ptree cfg, module_cfg, readers_cfg, reader1_cfg;

                reader1_cfg.add("name", "WIEGAND_4242");
                reader1_cfg.add("high", "GPIO_HIGH");
                reader1_cfg.add("low", "GPIO_LOW");

                readers_cfg.add_child("reader", reader1_cfg);
                module_cfg.add_child("readers", readers_cfg);

                cfg.add("name", "WIEGAND_READER");
                cfg.add_child("module_config", module_cfg);

        kernel_.config_manager().store_config("WIEGAND_READER", cfg);

        kernel_.module_manager().initModule("WIEGAND_READER");
        kernel_.module_manager().initModule("DOORMAN");
                INFO("Remote Control resuming normal operation");
        *message_out << "HO";
    }
    else
    {
        *message_out << "ENDPOINT SEEMS INVALID";
    }
}


static bool retrieved_all_config(const std::map<std::string, bool> &cfg)
{
    for (const auto &p : cfg)
    {
        if (!p.second)
            return false;
    }
    return true;
}

bool RemoteControl::gather_remote_config(zmqpp::socket &sock)
{
    std::map<std::string, bool> cfg;

    // list of name of remote module.
    std::vector<std::string> remote_modules;

    sock.send("MODULE_LIST");

    while (true)
    {
        zmqpp::message msg;
        bool ret = sock.receive(msg);
        assert(ret);
        std::string tmp;
        while (msg.remaining())
        {
        msg >> tmp;
            remote_modules.push_back(tmp);
    }
    }

    if (remote_modules != kernel_.module_manager().modules_names())
    {
        ERROR("MODULE LIST DIFFERS. THIS TYPE OF SYNC IS NOT YET SUPPORTED");
        return false;
    }

    for (const std::string &module_name : kernel_.module_manager().modules_names())
    {
        cfg[module_name] = false;
        zmqpp::message msg;
        msg << "MODULE_CONFIG" << module_name;
        sock.send(msg);
    }

    while (!retrieved_all_config(cfg))
    {
        zmqpp::message msg;


        sock.receive(msg);
        assert(msg.parts() == 3);

    }
return true;
}
