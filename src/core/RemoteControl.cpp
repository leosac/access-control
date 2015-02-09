#include <tools/log.hpp>
#include <zmqpp/curve.hpp>
#include "RemoteControl.hpp"
#include "kernel.hpp"
#include <boost/regex.hpp>
#include <cassert>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/property_tree/ptree_serialization.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <tools/XmlPropertyTree.hpp>
#include <core/config/RemoteConfigCollector.hpp>

using namespace Leosac;

RemoteControl::RemoteControl(zmqpp::context &ctx,
        Kernel &kernel,
        const boost::property_tree::ptree &cfg) :
        kernel_(kernel),
        socket_(ctx, zmqpp::socket_type::router),
        auth_(ctx),
        context_(ctx)
{
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

    command_handlers_["MODULE_CONFIG"] = std::bind(&RemoteControl::handle_module_config, this, std::placeholders::_1,
            std::placeholders::_2);

    command_handlers_["MODULE_LIST"] = std::bind(&RemoteControl::handle_module_list, this, std::placeholders::_1,
            std::placeholders::_2);

    command_handlers_["SYNC_FROM"] = std::bind(&RemoteControl::handle_sync_from, this, std::placeholders::_1,
            std::placeholders::_2);

    command_handlers_["SAVE"] = std::bind(&RemoteControl::handle_save, this, std::placeholders::_1,
            std::placeholders::_2);

    command_handlers_["GENERAL_CONFIG"] = std::bind(&RemoteControl::handle_general_config, this, std::placeholders::_1,
            std::placeholders::_2);

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
    rep << source;

    msg >> frame1;
    DEBUG("Cmd = {" << frame1 << "}");

    if (command_handlers_.find(frame1) != command_handlers_.end())
    {
        auto h = command_handlers_[frame1];

        bool ret = h(&msg, &rep);
        if (!ret)
        {
            // if the handler fails, that means the source message was incorrect.
            // therefore, the response shall be empty (except for the zmq id)
            assert(rep.parts() == 1);
            rep << "KO" << "Malformed message: " << frame1;
            NOTICE("Received malformed message on Remote Control Interface. Message type was " << frame1);
        }
    }
    else
    {
        rep << "KO" << "UNKOWN MESSAGE";
        WARN("UNKNOWN MESSAGE ON REMOTE CONTROL INTERFACE");
    }

    DEBUG("Sending response, " << rep.parts() << " frames");
    socket_.send(rep);
}

void RemoteControl::module_list(zmqpp::message *message_out)
{
    assert(message_out);
    for (const std::string &s : kernel_.module_manager().modules_names())
    {
        *message_out << s;
    }
}

void RemoteControl::module_config(const std::string &module, ConfigManager::ConfigFormat cfg_format, zmqpp::message *message_out)
{
    assert(message_out);

    // we need to make sure the module's name exist.
    std::vector<std::string> modules_names = kernel_.module_manager().modules_names();
    if (std::find(modules_names.begin(), modules_names.end(), module) != modules_names.end())
    {
        zmqpp::socket sock(context_, zmqpp::socket_type::req);
        sock.connect("inproc://module-" + module);

        // ask for a binary dump
        bool ret = sock.send(zmqpp::message() << "DUMP_CONFIG" << cfg_format);


        assert(ret);
        zmqpp::message rep;

        sock.receive(rep);
        *message_out << "OK";
        *message_out << module;

        // extract data from received configuration
        // fixme this is poor code and involve lots of copying.
        while (rep.remaining())
        {
            std::string tmp;
            rep >> tmp;
            *message_out << tmp;
        }
    }
    else
    {
        // if module with this name is not found
        ERROR("RemoteControl: Cannot retrieve local module configuration for {" << module << "}" <<
                "The module appears to not be loaded.");
        *message_out << "KO" << "Module not loaded, so config not available";
    }
}

void RemoteControl::general_config(ConfigManager::ConfigFormat cfg_format, zmqpp::message *msg_out)
{
    assert(msg_out);

    auto cfg = kernel_.config_manager().get_exportable_general_config();

    if (cfg_format == ConfigManager::ConfigFormat::BOOST_ARCHIVE)
    {
        std::ostringstream oss;
        boost::archive::text_oarchive archive(oss);
        boost::property_tree::save(archive, cfg, 1);
        msg_out->add("OK");
        msg_out->add(oss.str());
    }
    else
    {
        msg_out->add("OK");
        msg_out->add(Tools::propertyTreeToXml(cfg));
    }
}

void RemoteControl::sync_from(const std::string &endpoint, const std::string &remote_server_pk,
        uint8_t sync_general_config, zmqpp::message *message_out)
{
    assert(message_out);
    RemoteConfigCollector collector(context_, endpoint, remote_server_pk);

    std::string error;
    if (collector.fetch_config(&error))
    {

        if (sync_general_config)
        {
            INFO("Also syncing general configuration.");
            // syncing the global configure requires restart.
            kernel_.config_manager().set_kconfig(collector.general_config());
            kernel_.restart_later();
        }

        kernel_.module_manager().stopModules();

        // module manager code is half-broken.
        // so for now we iterate over the module to load, later we will be able
        // to just call initModules();
        for (const auto &name : collector.modules_list())
        {
            kernel_.config_manager().store_config(name, collector.module_config(name));
            if (!kernel_.module_manager().initModule(name))
            {
                // maybe it wasn't preloaded in the module manager.
                // attempt to load dll and try again.
                kernel_.module_manager().loadModule(name);
                bool r = kernel_.module_manager().initModule(name);
                assert(r);
            }
        }
        *message_out << "OK";
    }
    else
    {
        *message_out << "KO" << error;
    }
}

bool RemoteControl::handle_module_config(zmqpp::message *msg_in, zmqpp::message *msg_out)
{
    assert(msg_in);
    assert(msg_out);

    if (msg_in->remaining() >= 2)
    {
        std::string module_name;
        ConfigManager::ConfigFormat format;
        *msg_in >> module_name >> format;
        module_config(module_name, format, msg_out);
        return true;
    }
    return false;
}

bool RemoteControl::handle_module_list(zmqpp::message *msg_in, zmqpp::message *msg_out)
{
    assert(msg_in);
    assert(msg_out);

    if (msg_in->remaining() == 0)
    {
        module_list(msg_out);
        return true;
    }
        return false;
}

bool RemoteControl::handle_sync_from(zmqpp::message *msg_in, zmqpp::message *msg_out)
{
    assert(msg_in);
    assert(msg_out);

    uint8_t autocommit;

    if (msg_in->remaining() == 4)
    {
        std::string endpoint;
        std::string remote_server_pubkey;
        uint8_t sync_general_config;

        *msg_in >> endpoint;
        *msg_in >> autocommit;
        *msg_in >> remote_server_pubkey;
        *msg_in >> sync_general_config;
        sync_from(endpoint, remote_server_pubkey, sync_general_config, msg_out);

        if (autocommit)
        {
            INFO("Saving configuration to disk after synchronization.");
            kernel_.save_config();
        }
        return true;
    }
    return false;
}

bool RemoteControl::handle_save(zmqpp::message *msg_in, zmqpp::message *msg_out)
{
    assert(msg_in);
    assert(msg_out);

    if (msg_in->remaining() == 0)
    {
        if (kernel_.save_config())
            *msg_out << "OK";
        else
            *msg_out << "KO" << "Saving config failed for some unkown reason.";

        return true;
    }
    return false;
}

bool RemoteControl::handle_general_config(zmqpp::message *msg_in, zmqpp::message *msg_out)
{
    assert(msg_in);
    assert(msg_out);

    if (msg_in->remaining() == 1)
    {
        ConfigManager::ConfigFormat format;
        *msg_in >> format;
        general_config(format, msg_out);
        return true;
    }
    return false;
}
