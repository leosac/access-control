/*
    Copyright (C) 2014-2016 Islog

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

#include "RplethModule.hpp"
#include "core/auth/Auth.hpp"
#include "core/auth/WiegandCard.hpp"
#include "hardware/FWiegandReader.hpp"
#include "rplethprotocol.hpp"
#include "tools/log.hpp"
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/property_tree/ptree_serialization.hpp>
#include <netinet/in.h>

using namespace Leosac::Module::Rpleth;

RplethModule::RplethModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                           const boost::property_tree::ptree &cfg,
                           CoreUtilsPtr utils)
    : BaseModule(ctx, pipe, cfg, utils)
    , ctx_(ctx)
    , server_(ctx, zmqpp::socket_type::stream)
    , bus_sub_(ctx, zmqpp::socket_type::sub)
    , core_(ctx, zmqpp::socket_type::req)
    , reader_(nullptr)
    , stream_mode_(false)
{
    core_.connect("inproc://leosac-kernel");
    process_config();
    bus_sub_.connect("inproc://zmq-bus-pub");
    bus_sub_.subscribe("S_" + reader_->name());
    reactor_.add(server_, std::bind(&RplethModule::handle_socket, this));
    reactor_.add(bus_sub_, std::bind(&RplethModule::handle_wiegand_event, this));
}

RplethModule::~RplethModule()
{
}

void RplethModule::process_config()
{
    boost::property_tree::ptree module_config = config_.get_child("module_config");

    uint16_t port           = module_config.get<uint16_t>("port", 4242);
    std::string reader_name = module_config.get_child("reader").data();
    stream_mode_            = module_config.get<bool>("stream_mode", true);

    INFO("Rpleth module will bind to "
         << port << " and will control the device nammed " << reader_name
         << "Stream mode = " << stream_mode_);
    reader_ = std::unique_ptr<Hardware::FWiegandReader>(
        new Hardware::FWiegandReader(ctx_, reader_name));
    server_.bind("tcp://*:" + std::to_string(port));
}

void RplethModule::handle_socket()
{
    zmqpp::message msg;
    std::string identity;
    std::string content;

    server_.receive(msg);
    msg >> identity >> content;

    if (content.size() == 0)
    {
        // handle special 0 length message that indicates connection / disconnection.
        if (client_connected(
                identity)) // client exists so this is a disconnection message.
        {
            INFO("client disconnected");
            clients_.erase(identity);
            if (client_failed(identity))
                failed_clients_.erase(std::remove(failed_clients_.begin(),
                                                  failed_clients_.end(), identity),
                                      failed_clients_.end());
        }
        else
        {
            INFO("Client connected");
            clients_[identity];
        }
        return;
    }
    if (client_failed(identity))
        return;
    assert(clients_.count(identity) && !client_failed(identity));
    clients_[identity].write(reinterpret_cast<const uint8_t *>(content.c_str()),
                             content.size());
    if (handle_client_msg(identity, clients_[identity]) == false)
        failed_clients_.push_back(identity);
}

bool RplethModule::handle_client_msg(const std::string &client_identity,
                                     CircularBuffer &buf)
{
    RplethPacket packet(RplethPacket::Sender::Client);

    do
    {
        std::array<uint8_t, buffer_size> buffer;
        packet = RplethProtocol::decodeCommand(buf);
        if (!packet.isGood)
            break;
        RplethPacket response = handle_client_packet(packet);
        if (response.command == RplethProtocol::HIDCommands::Greenled ||
            response.command == RplethProtocol::HIDCommands::Beep)
            continue;
        std::size_t size =
            RplethProtocol::encodeCommand(response, &buffer[0], buffer_size);

        zmqpp::message msg;
        msg << client_identity;
        msg.add_raw(&buffer[0], size);
        if (!server_.send(msg, true))
        {
            // would block: peer is probably disconnected already.
            return false;
        }
    } while (packet.isGood && buf.toRead());
    return true;
}

RplethPacket RplethModule::handle_client_packet(RplethPacket packet)
{
    RplethPacket response = packet;
    try
    {
        response.sender = RplethPacket::Sender::Server;
        if (response.type == RplethProtocol::Rpleth &&
            response.command == RplethProtocol::Ping)
        {
            response.status = RplethProtocol::Success;
        }
        else if (response.type == RplethProtocol::TypeCode::HID &&
                 response.command == RplethProtocol::HIDCommands::Greenled)
            rpleth_greenled(packet);
        else if (response.type == RplethProtocol::TypeCode::HID &&
                 response.command == RplethProtocol::HIDCommands::Beep)
            rpleth_beep(packet);
        else if (response.type == RplethProtocol::TypeCode::HID &&
                 response.command == RplethProtocol::HIDCommands::SendCards)
            rpleth_send_cards(packet);
        else if (response.type == RplethProtocol::TypeCode::HID &&
                 response.command == RplethProtocol::HIDCommands::ReceiveCardsWaited)
            response = rpleth_receive_cards(response);
        else if (response.type == RplethProtocol::TypeCode::Rpleth &&
                 response.command == RplethProtocol::RplethCommands::DHCPState)
            response = get_dhcp_state();
        else if (response.type == RplethProtocol::TypeCode::Rpleth &&
                 response.command == RplethProtocol::RplethCommands::SetDHCP)
            response = set_dhcp_state(response);
        else if (response.type == RplethProtocol::TypeCode::Rpleth &&
                 response.command == RplethProtocol::RplethCommands::SetIP)
            response = set_reader_ip(response);
        else if (response.type == RplethProtocol::TypeCode::Rpleth &&
                 response.command == RplethProtocol::RplethCommands::SetSubnet)
            response = set_reader_netmask(response);
        else if (response.type == RplethProtocol::TypeCode::Rpleth &&
                 response.command == RplethProtocol::RplethCommands::SetGateway)
            response = set_reader_gw(response);
        else if (response.type == RplethProtocol::TypeCode::Rpleth &&
                 response.command == RplethProtocol::RplethCommands::Reset)
            restart_reader();
        else
        {
            WARN("Unhandled packet.");
            response.status = RplethProtocol::Success; // Default response
        }
        return response;
    }
    catch (std::exception &e)
    {
        ERROR("Exception while handling rpleth packet: " << e.what());
    }
    response.status = RplethProtocol::Failed;
    return response;
}

void RplethModule::handle_wiegand_event()
{
    zmqpp::message msg;
    std::string card_id;
    std::string src; // object that sent event
    Leosac::Auth::SourceType type;
    int nb_bit_read;

    bus_sub_.receive(msg);
    msg >> src >> type;
    assert(type == Leosac::Auth::SourceType::SIMPLE_WIEGAND);
    msg >> card_id >> nb_bit_read;

    DEBUG("Rpleth module registered card with id "
          << card_id << " and " << nb_bit_read << " significants bits");

    if (stream_mode_)
        cards_read_stream_.push_back(std::make_pair(card_id, nb_bit_read));

    card_id.erase(std::remove(card_id.begin(), card_id.end(), ':'), card_id.end());
    if (std::find(cards_pushed_.begin(), cards_pushed_.end(), card_id) !=
        cards_pushed_.end())
    {
        cards_read_.push_back(card_id);
    }
    cards_read_.unique();
    rpleth_publish_card();
}

void RplethModule::rpleth_send_cards(const RplethPacket &packet)
{
    auto itr_start                             = packet.data.begin();
    std::vector<Byte>::const_iterator my_start = itr_start;
    std::vector<Byte>::const_iterator it;

    WARN("Should not be here");
    cards_pushed_.clear();
    cards_read_.clear();

    while (my_start != packet.data.end())
    {
        it = std::find(itr_start, packet.data.end(), '|');
        std::string card;
        while (my_start != packet.data.end() && my_start != it)
        {
            card += *my_start;
            my_start++;
        }
        cards_pushed_.push_back(card);
        if (my_start == packet.data.end())
            break;
        else
            my_start++;
        itr_start = ++it;
    }
    cards_pushed_.unique();
}

RplethPacket RplethModule::rpleth_receive_cards(const RplethPacket &packet)
{
    std::list<std::string> to_send;
    RplethPacket response = packet;
    WARN("Should not be here");
    DEBUG("Packet size = " << packet.data.size());
    if (packet.data.size() != 1)
    {
        WARN("Invalid Packet");
        return response;
    }
    if (packet.data[0] == 0x01)
    {
        DEBUG("Present list");
        // send present list
        to_send = cards_read_;
    }
    else
    {
        DEBUG("Absent list");
        // send absent list
        to_send     = cards_pushed_;
        auto lambda = [this](const std::string &str) -> bool {
            // if entry is not in cards_read_ means user was absent, do not remove
            // him
            bool found = std::find(cards_read_.begin(), cards_read_.end(), str) !=
                         cards_read_.end();
            return found;
        };
        to_send.erase(std::remove_if(to_send.begin(), to_send.end(), lambda),
                      to_send.end());
    }
    // we reserve approximately what we need, just to avoid useless memory
    // allocations.
    std::vector<Byte> data;
    data.reserve(to_send.size() * 8);
    for (auto &card : to_send)
    {
        // we need to convert the card (ff:ae:32:00) to something like "ffae3200"
        card.erase(std::remove(card.begin(), card.end(), ':'), card.end());
        data.insert(data.end(), card.begin(), card.end());
        data.push_back('|');
    }
    response.data    = data;
    response.dataLen = data.size();
    return response;
}

void RplethModule::rpleth_beep(const RplethPacket &packet)
{
    if (packet.data.size() > 0)
    {
        if (packet.data[0] == 0x01)
            reader_->buzzerOn();
        else if (packet.data[0] == 0x00)
            reader_->buzzerOff();
        else
            WARN("Malformed (Rpleth Beep) packet. Data byte is invalid");
    }
    else
        WARN("Malformed (Rpleth Beep) packet. Not enough data");
}

void RplethModule::rpleth_greenled(const RplethPacket &packet)
{
    if (packet.data.size() > 0)
    {
        if (packet.data[0] == 0x01)
            reader_->greenLedOn();
        else if (packet.data[0] == 0x00)
            reader_->greenLedOff();
        else
            WARN("Malformed (Rpleth GreenLed) packet. Data byte is invalid");
    }
    else
        WARN("Malformed (Rpleth GreenLed) packet. Not enough data");
}

bool RplethModule::client_connected(const std::string &identity) const
{
    return clients_.count(identity);
}

bool RplethModule::client_failed(const std::string &identity) const
{
    return (std::find(failed_clients_.begin(), failed_clients_.end(), identity) !=
            failed_clients_.end());
}

void RplethModule::rpleth_publish_card()
{
    for (auto const &card_pair : cards_read_stream_)
    {
        for (auto &client : clients_)
        {
            zmqpp::message msg;
            RplethPacket packet(RplethPacket::Sender::Server);

            msg << client.first;
            packet.data    = card_convert_from_text(card_pair);
            packet.status  = RplethProtocol::Success;
            packet.type    = RplethProtocol::HID;
            packet.command = RplethProtocol::Badge;
            packet.dataLen = packet.data.size();

            std::array<uint8_t, 64> buf;
            std::size_t size;
            size = RplethProtocol::encodeCommand(packet, &buf[0], buf.size());
            msg.add_raw(&buf, size);
            if (!server_.send(msg, true))
                failed_clients_.push_back(client.first);
        }
    }
    cards_read_stream_.clear();
}

static uint64_t htonll(uint64_t value)
{
    int num = 42;
    if (*(char *)&num == 42)
    {
        uint32_t high_part = htonl((uint32_t)(value >> 32));
        uint32_t low_part  = htonl((uint32_t)(value & 0xFFFFFFFFLL));
        return (((uint64_t)low_part) << 32) | high_part;
    }
    else
    {
        return value;
    }
}

std::vector<uint8_t>
RplethModule::card_convert_from_text(const std::pair<std::string, int> &card_info)
{
    Auth::WiegandCard wc(card_info.first, card_info.second);
    std::vector<uint8_t> ret;

    auto num = wc.to_raw_int();
    // This will go over the network. So we have to convert to network byte order.
    num = htonll(num);

    ret.resize(sizeof(num));
    std::memcpy(&ret[0], &num, sizeof(num));
    return ret;
}

RplethPacket RplethModule::get_dhcp_state()
{
    auto network_cfg = get_network_config();
    RplethPacket response(RplethPacket::Sender::Server);

    response.type    = RplethProtocol::TypeCode::Rpleth;
    response.command = RplethProtocol::RplethCommands::DHCPState;
    try
    {
        if (network_cfg.get<bool>("enabled"))
        {
            response.status = RplethProtocol::Success;
            response.data   = network_cfg.get<bool>("dhcp")
                                ? std::vector<uint8_t>({1})
                                : std::vector<uint8_t>({0});
            response.dataLen = 1;
        }
        else // network not handled by leosac. return failure.
        {
            response.status = RplethProtocol::Failed;
        }
    }
    catch (const std::exception &e)
    {
        ERROR("Exception while getting DHCP state: " << e.what());
    }
    return response;
}

RplethPacket RplethModule::set_dhcp_state(const RplethPacket &p)
{
    auto network_cfg = get_network_config();
    RplethPacket response(RplethPacket::Sender::Server);

    response.status  = RplethProtocol::Failed;
    response.type    = RplethProtocol::TypeCode::Rpleth;
    response.command = RplethProtocol::RplethCommands::SetDHCP;

    if (p.dataLen < 1)
        ERROR("Invalid Rpleth Packet");
    else if (network_cfg.get<bool>("enabled"))
    {
        network_cfg.erase("dhcp");
        network_cfg.put("dhcp", p.data[0] ? true : false);
        if (push_network_config(network_cfg))
            response.status = RplethProtocol::Success;
        else
            WARN("Failed to update network config.");
    }
    else
        NOTICE("Network not managed by Leosac, doing nothing.");
    return response;
}

RplethPacket RplethModule::set_reader_ip(const RplethPacket &p)
{
    auto network_cfg = get_network_config();
    RplethPacket response(RplethPacket::Sender::Server);

    response.status  = RplethProtocol::Failed;
    response.type    = RplethProtocol::TypeCode::Rpleth;
    response.command = RplethProtocol::RplethCommands::SetIP;

    if (p.dataLen < 4)
        ERROR("Invalid Rpleth Packet");
    else if (network_cfg.get<bool>("enabled"))
    {
        std::ostringstream oss;
        for (unsigned char i = 0; i < 4; ++i)
        {
            oss << std::to_string(p.data[i]);
            if (i != 3)
                oss << ".";
        }
        INFO("new ip  = {" << oss.str() << "}");
        network_cfg.erase("default_ip");
        network_cfg.put("default_ip", oss.str());
        if (push_network_config(network_cfg))
            response.status = RplethProtocol::Success;
        else
            WARN("Failed to update network config.");
    }
    else
        NOTICE("Network not managed by Leosac, doing nothing.");
    return response;
}

RplethPacket RplethModule::set_reader_netmask(const RplethPacket &p)
{
    auto network_cfg = get_network_config();
    RplethPacket response(RplethPacket::Sender::Server);

    response.status  = RplethProtocol::Failed;
    response.type    = RplethProtocol::TypeCode::Rpleth;
    response.command = RplethProtocol::RplethCommands::SetSubnet;

    if (p.dataLen < 4)
        ERROR("Invalid Rpleth Packet");
    else if (network_cfg.get<bool>("enabled"))
    {
        std::ostringstream oss;
        for (unsigned char i = 0; i < 4; ++i)
        {
            oss << std::to_string(p.data[i]);
            if (i != 3)
                oss << ".";
        }
        network_cfg.erase("netmask");
        network_cfg.put("netmask", oss.str());
        if (push_network_config(network_cfg))
            response.status = RplethProtocol::Success;
        else
            WARN("Failed to update network config.");
    }
    else
        NOTICE("Network not managed by Leosac, doing nothing.");
    return response;
}

RplethPacket RplethModule::set_reader_gw(const RplethPacket &p)
{
    auto network_cfg = get_network_config();
    RplethPacket response(RplethPacket::Sender::Server);

    response.status  = RplethProtocol::Failed;
    response.type    = RplethProtocol::TypeCode::Rpleth;
    response.command = RplethProtocol::RplethCommands::SetGateway;

    if (p.dataLen < 4)
        ERROR("Invalid Rpleth Packet");
    else if (network_cfg.get<bool>("enabled"))
    {
        std::ostringstream oss;
        for (unsigned char i = 0; i < 4; ++i)
        {
            oss << std::to_string(p.data[i]);
            if (i != 3)
                oss << ".";
        }
        network_cfg.erase("gateway");
        network_cfg.put("gateway", oss.str());
        if (push_network_config(network_cfg))
            response.status = RplethProtocol::Success;
        else
            WARN("Failed to update network config.");
    }
    else
        NOTICE("Network not managed by Leosac, doing nothing.");
    return response;
}

bool RplethModule::push_network_config(boost::property_tree::ptree const &tree)
{
    std::ostringstream oss;
    boost::archive::binary_oarchive archive(oss);
    boost::property_tree::save(archive, tree, 1);
    core_.send(zmqpp::message() << "SET_NETCONFIG" << oss.str());

    std::string ret;
    core_.receive(ret);
    return ret == "OK" ? true : false;
}

boost::property_tree::ptree RplethModule::get_network_config()
{
    boost::property_tree::ptree network_config;
    std::string data;

    core_.send("GET_NETCONFIG");

    zmqpp::message response;
    core_.receive(response);
    assert(response.parts() == 1);

    response >> data;
    std::istringstream iss(data);
    boost::archive::binary_iarchive archive(iss);
    boost::property_tree::load(archive, network_config, 1);

    return network_config;
}

void RplethModule::restart_reader()
{
    std::string ret;
    core_.send("RESTART");
    core_.receive(ret);
}
