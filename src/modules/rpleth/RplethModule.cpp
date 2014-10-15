#include <tools/log.hpp>
#include <hardware/device/FWiegandReader.hpp>
#include "RplethModule.hpp"
#include "rplethprotocol.hpp"

RplethModule::RplethModule(zmqpp::context &ctx,
        zmqpp::socket *pipe,
        const boost::property_tree::ptree &cfg) :
        BaseModule(ctx, pipe, cfg),
        ctx_(ctx),
        server_(ctx, zmqpp::socket_type::stream),
        reader_(nullptr)
{
    process_config();
    reactor_.add(server_, std::bind(&RplethModule::handle_socket, this));
}

RplethModule::~RplethModule()
{
    delete reader_;
}

void RplethModule::process_config()
{
    boost::property_tree::ptree module_config = config_.get_child("module_config");

    uint16_t port = module_config.get<uint16_t>("port", 4242);
    std::string reader_name = module_config.get_child("reader").data();

    LOG() << "Rpleth module will bind to " << port << " and will control the device nammed " << reader_name;
    reader_ = new FWiegandReader(ctx_, reader_name);
    server_.bind("tcp://*:" + port);
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
        // connect or disconnect
        if (clients_.count(identity))
        {
            LOG() << "client disconnected";
            clients_.erase(identity);
        }
        else
        {
            LOG() << "Client conncted";
            clients_[identity];
        }
        return;
    }
    assert(clients_.count(identity));
    // fixme proper cast
    clients_[identity].write(reinterpret_cast<const uint8_t *> (content.c_str()), content.size());
    handle_client_msg(identity, clients_[identity]);
}

void RplethModule::handle_client_msg(const std::string &client_identity, CircularBuffer &buf)
{
    RplethPacket packet(RplethPacket::Sender::Client);

    do
    {
        std::array<uint8_t, buffer_size> buffer;
        packet = RplethProtocol::decodeCommand(buf);
        if (!packet.isGood)
            break;
        RplethPacket response = handle_client_packet(packet);
        std::size_t size = RplethProtocol::encodeCommand(response, &buffer[0], buffer_size);

        zmqpp::message msg;
        msg << client_identity;
        msg.add_raw(&buffer[0], size);
        server_.send(msg);
    }
    while (packet.isGood && buf.toRead());
}

RplethPacket RplethModule::handle_client_packet(RplethPacket packet)
{
    RplethPacket response = packet;

    response.sender = RplethPacket::Sender::Server;
    if (response.type == RplethProtocol::Rpleth && response.command == RplethProtocol::Ping)
    {
        response.status = RplethProtocol::Success;
    }
    else if (response.type == RplethProtocol::TypeCode::HID && response.command == RplethProtocol::HIDCommands::Greenled)
    {
        assert(packet.data.size() > 0);
        if (packet.data[0] == 0x01)
            reader_->greenLedOn();
        else if (packet.data[0] == 0x00)
            reader_->greenLedOff();
        else
            LOG() << "Invalid packet";
    }
    else if (response.type == RplethProtocol::TypeCode::HID && response.command == RplethProtocol::HIDCommands::Beep)
    {
        assert(packet.data.size() > 0);
        if (packet.data[0] == 0x01)
            reader_->buzzerOn();
        else if (packet.data[0] == 0x00)
            reader_->buzzerOff();
        else
            LOG() << "Invalid packet";
    }
    else
    {
        response.status = RplethProtocol::Success; // Default response
    }
    return (response);
}
