/*
    Copyright (C) 2014-2015 Islog

    This file is part of Leosac.

    Leosac is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leosac is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "RemoteControl.hpp"
#include "tools/log.hpp"
#include "kernel.hpp"
#include "tools/XmlPropertyTree.hpp"
#include "core/config/RemoteConfigCollector.hpp"
#include "core/tasks/FetchRemoteConfig.hpp"
#include "core/tasks/SyncConfig.hpp"
#include "core/tasks/RemoteControlAsyncResponse.hpp"
#include <cassert>
#include <zmqpp/curve.hpp>
#include <boost/regex.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/property_tree/ptree_serialization.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "core/CoreUtils.hpp"

using namespace Leosac;

RemoteControl::RemoteControl(zmqpp::context &ctx, Kernel &kernel,
                             const boost::property_tree::ptree &cfg)
    : kernel_(kernel)
    , socket_(ctx, zmqpp::socket_type::router)
    , auth_(ctx)
    , context_(ctx)
    , security_(cfg)
{
    auth_.configure_curve("CURVE_ALLOW_ANY");
    process_config(cfg);
}

void RemoteControl::process_config(const boost::property_tree::ptree &cfg)
{
    int port = cfg.get<int>("port");

    secret_key_ = cfg.get<std::string>("secret_key");
    public_key_ = cfg.get<std::string>("public_key");

    INFO("Enabling Remote Control:"
         << "\n\t "
         << "Port: " << port << "\n\t "
         << "Public Key: " << public_key_ << "\n\t "
         << "Private Key: " << secret_key_);

    command_handlers_["MODULE_CONFIG"] =
        std::bind(&RemoteControl::handle_module_config, this, std::placeholders::_1,
                  std::placeholders::_2);

    command_handlers_["MODULE_LIST"] =
        std::bind(&RemoteControl::handle_module_list, this, std::placeholders::_1,
                  std::placeholders::_2);

    command_handlers_["SYNC_FROM"] =
        std::bind(&RemoteControl::handle_sync_from, this, std::placeholders::_1,
                  std::placeholders::_2);

    command_handlers_["SAVE"] =
        std::bind(&RemoteControl::handle_save, this, std::placeholders::_1,
                  std::placeholders::_2);

    command_handlers_["GENERAL_CONFIG"] =
        std::bind(&RemoteControl::handle_general_config, this, std::placeholders::_1,
                  std::placeholders::_2);

    command_handlers_["CONFIG_VERSION"] =
        std::bind(&RemoteControl::handle_config_version, this, std::placeholders::_1,
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
    std::string frame1;
    socket_.receive(msg);

    assert(msg.parts() > 1);
    msg >> current_client_idt_;
    rep << current_client_idt_;

    msg.pop_front();         // otherwise getting the "User-Id" property
    msg.reset_read_cursor(); // wont work.

    msg >> frame1;
    DEBUG("Remote Control command: " << frame1 << " with " << msg.parts()
                                     << " parts");

    std::string user_pubkey;
    bool ret = msg.get_property("User-Id", user_pubkey);
    assert(ret);

    if (command_handlers_.find(frame1) != command_handlers_.end())
    {

        if (security_.allow_request(user_pubkey, frame1))
        {
            auto h = command_handlers_[frame1];

            ret = h(&msg, &rep);
            if (!ret)
            {
                // if the handler fails, that means the source message was incorrect.
                // therefore, the response shall be empty (except for the zmq id)
                assert(rep.parts() == 1);
                rep << "KO"
                    << "Malformed message: " << frame1;
                WARN("Received malformed message on Remote Control Interface. "
                     "Message type was "
                     << frame1);
            }
        }
        else
        {
            WARN("Request denied. Insuficient permission for user "
                 << user_pubkey << ". Command was: " << frame1);
            rep << "KO"
                << "Insuficient permission";
        }
    }
    else
    {
        rep << "KO"
            << "UNKOWN MESSAGE";
        WARN("Unknown message on Remote Control interface");
    }

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

void RemoteControl::module_config(const std::string &module,
                                  ConfigManager::ConfigFormat cfg_format,
                                  zmqpp::message *message_out)
{
    assert(message_out);

    // we need to make sure the module's name exist.
    std::vector<std::string> modules_names =
        kernel_.module_manager().modules_names();
    if (std::find(modules_names.begin(), modules_names.end(), module) !=
        modules_names.end())
    {
        zmqpp::socket sock(context_, zmqpp::socket_type::req);
        sock.connect("inproc://module-" + module);

        bool ret = sock.send(zmqpp::message() << "DUMP_CONFIG" << cfg_format);
        ASSERT_LOG(ret, "Failed to send");

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
        ERROR("RemoteControl: Cannot retrieve local module configuration for {"
              << module << "}"
              << "The module appears to not be loaded.");
        *message_out << "KO"
                     << "Module not loaded, so config not available";
    }
}

void RemoteControl::general_config(ConfigManager::ConfigFormat cfg_format,
                                   zmqpp::message *msg_out)
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

bool RemoteControl::handle_module_config(zmqpp::message *msg_in,
                                         zmqpp::message *msg_out)
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

bool RemoteControl::handle_module_list(zmqpp::message *msg_in,
                                       zmqpp::message *msg_out)
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


        auto fetch_task = std::make_shared<Tasks::FetchRemoteConfig>(
            endpoint, remote_server_pubkey);

        auto sync_task = std::make_shared<Tasks::SyncConfig>(
            kernel_, fetch_task, sync_general_config, autocommit);
        auto *sched = &kernel_.core_utils()->scheduler();

        auto success_response_task =
            std::make_shared<Tasks::RemoteControlAsyncResponse>(
                current_client_idt_,
                zmqpp::message() << "Success" << sync_task->get_guid(), socket_);

        auto failure_response_task =
            std::make_shared<Tasks::RemoteControlAsyncResponse>(
                current_client_idt_,
                zmqpp::message() << "Failed" << sync_task->get_guid(), socket_);

        auto abort_response_task =
            std::make_shared<Tasks::RemoteControlAsyncResponse>(
                current_client_idt_,
                zmqpp::message() << "Aborted" << sync_task->get_guid(), socket_);

        sync_task->set_on_success([=]() {
            success_response_task->run();
            ASSERT_LOG(success_response_task->succeed(), "TASK FAILED");
        });
        sync_task->set_on_failure([=]() {
            failure_response_task->run();
            ASSERT_LOG(failure_response_task->succeed(), "TASK FAILED");
        });
        fetch_task->set_on_failure([=]() {
            // not run on main thread, need to be queued.
            sched->enqueue(abort_response_task, TargetThread::MAIN);
        });

        fetch_task->set_on_success([=]() {
            DEBUG("FETCH TASK COMPLETE. WILL QUEUE SYNC_CONFIG");
            sched->enqueue(sync_task, TargetThread::MAIN);
        });

        kernel_.core_utils()->scheduler().enqueue(fetch_task, TargetThread::POOL);

        *msg_out << "DELAYED" << sync_task->get_guid();
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
            *msg_out << "KO"
                     << "Saving config failed for some unkown reason.";

        return true;
    }
    return false;
}

bool RemoteControl::handle_general_config(zmqpp::message *msg_in,
                                          zmqpp::message *msg_out)
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

bool RemoteControl::handle_config_version(zmqpp::message *msg_in,
                                          zmqpp::message *msg_out)
{
    assert(msg_in);
    assert(msg_out);

    if (msg_in->remaining() == 0)
    {
        *msg_out << static_cast<uint64_t>(kernel_.config_manager().config_version());
        return true;
    }
    return false;
}

void RemoteControl::update()
{
}
