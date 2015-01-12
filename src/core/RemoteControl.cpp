#include <tools/log.hpp>
#include <zmqpp/curve.hpp>
#include "RemoteControl.hpp"
#include "kernel.hpp"
#include <boost/regex.hpp>
#include <cassert>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/property_tree/ptree_serialization.hpp>

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
    else
        assert(0);

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

    // fixme just fix module
    if (module == "STDIN_CONTROLLER")
    {
        *message_out << "KO";
        return;
    }

    // we need to make sure the module's name exist.
    std::vector<std::string> modules_names = kernel_.module_manager().modules_names();
    if (std::find(modules_names.begin(), modules_names.end(), module) != modules_names.end())
    {
        zmqpp::socket sock(context_, zmqpp::socket_type::req);
        std::string serialized_cfg;
        DEBUG("FOUND " << module);
        sock.connect("inproc://module-" + module);
        // ask for a binary dump
        bool ret = sock.send(zmqpp::message() << "DUMP_CONFIG" << uint8_t('0'));
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

    auto kp = zmqpp::curve::generate_keypair();
    sock.set(zmqpp::socket_option::curve_secret_key, kp.secret_key);
    sock.set(zmqpp::socket_option::curve_public_key, kp.public_key);
    sock.set(zmqpp::socket_option::curve_server_key, "TJz$:^DbZvFN@wv/ct&[Su6Nnu6w!fMGHEcIttyT");
    sock.set(zmqpp::socket_option::linger, 0);

    if (validate_endpoint(endpoint))
    {
        INFO("Attempting to fetch config from {" << endpoint << "}");
        sock.connect(endpoint);

        INFO("Remote Control module entering blocking mode...");

        // list of module to stop
        std::list<std::string> stop_list;

        // list of module to start.
        std::list<std::string> start_list;

        if (gather_remote_config(sock, start_list, stop_list))
        {
            for (const std::string &m : stop_list)
                kernel_.module_manager().stopModule(m);
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));

            for (const std::string &m : start_list)
                if (!kernel_.module_manager().initModule(m))
                {
                    kernel_.module_manager().loadModule(kernel_.config_manager().load_config(m));
                    bool r = kernel_.module_manager().initModule(m);
                    assert(r);
                }

            INFO("Remote Control resuming normal operation");
            *message_out << "HO";
        }
        else
        {
            *message_out << "Failed to synchronise configuration.";
        }
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
        //fix me temporary fix to disable this module as we cant get its config
        if (p.first == "STDIN_CONTROLLER")
            continue;
        if (!p.second)
        {
            return false;
    }
    }
    return true;
}

bool RemoteControl::gather_remote_config(zmqpp::socket &sock, std::list<std::string> &start_list,
        std::list<std::string> &stop_list)
{
    std::map<std::string, bool> cfg;

    // list of name of remote module.
    std::list<std::string> remote_modules;

    // first get the list of module currently running on the remote
    // host. This is done in a blocking way, with a 3s timeout.
    {
        sock.send("MODULE_LIST");

        zmqpp::poller p;
        p.add(sock);
        p.poll(3000);
        if (p.has(sock))
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
        else
        {
            // no response after 3s.
            WARN("Could'nt get remote module_list in time. Aborting synchronisation");
            return false;
        }
    }

    // we want to retrieve the configure of the remote module.
    // so we dont really care about our current module.
    for (const std::string &module_name : remote_modules)
    {
        cfg[module_name] = false;
        zmqpp::message msg;
        msg << "MODULE_CONFIG" << module_name;
        INFO("Request remote config for " << module_name);
        sock.send(msg);
    }

    // we want to stop all our module that weren't running on the remote host.
    for (const std::string &my_module : kernel_.module_manager().modules_names())
    {
            stop_list.push_back(my_module);
    }

    // we want to start all remote module.
    start_list.insert(start_list.end(), remote_modules.begin(), remote_modules.end());

    // fixme would break in some case.
    while (!retrieved_all_config(cfg))
    {
        zmqpp::message msg;
        std::string ok;
        std::string module;
        std::string binary_cfg;

        DEBUG("HOHO");
        sock.receive(msg);

        msg >> ok;
        if (ok == "OK")
        {
            assert(msg.parts() == 3);

            msg >> module >> binary_cfg;
            cfg[module] = true;

            INFO("Updating ConfigManager configuration for module " << module);
            std::istringstream iss(binary_cfg);
            boost::archive::text_iarchive archive(iss);
            boost::property_tree::ptree cfg_tree;
            boost::property_tree::load(archive, cfg_tree, 1);
            cfg[module] = true;
            kernel_.config_manager().store_config(module, cfg_tree);
        }
        else
        {
            ERROR("Error while retrieving config for module.");
        }
    }
    INFO("SUCCESS");
    return true;
}
