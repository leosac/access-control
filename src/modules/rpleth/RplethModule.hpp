#pragma once

#include "modules/BaseModule.hpp"
#include "hardware/device/FWiegandReader.hpp"
#include "rplethpacket.hpp"
#include <memory>
#include <vector>
#include <modules/rpleth/network/circularbuffer.hpp>

/**
* This module adds support for Rpleth protocol.
*
* It allows remote control of Wiegand Reader devices.

* ### Configuration Options

* Options    | Options  | Options    | Description                                            | Mandatory
* -----------|----------|------------|--------------------------------------------------------|-----------
* port       |          |            | Which port should we bind to ?                         | YES
* reader     |          |            | Name of the wiegand reader the module handles          | YES
*/
class RplethModule : public BaseModule
{
public:
    RplethModule(zmqpp::context &ctx,
            zmqpp::socket *pipe,
            const boost::property_tree::ptree &cfg);

    ~RplethModule();

    RplethModule(const RplethModule &) = delete;

    RplethModule &operator=(const RplethModule &) = delete;

private:
    void process_config();

    static constexpr int buffer_size = 2048;

    /**
    * Try to handle a client message. This is called when we received any amount of data, for any client.
    * If there isn't enough data to handle a rpleth message, this does nothing.
    */
    void handle_client_msg(const std::string &client_identity, CircularBuffer &buf);

    /**
    * If we successfully built a packet, lets handle it.
    */
    RplethPacket handle_client_packet(RplethPacket packet);

    /**
    * Handle data available on server socket.
    */
    void handle_socket();

    /**
    * Handle the SendCards command: we will store the list of received card somewhere safe.
    * Cards are text data, separated by a pipe.
    */
    void handle_send_cards(RplethPacket packet);

    /**
    * Handle the ReceiveUnpresentedCards command.
    * This sends cards that were read to the Rpleth client.
    * @return a response packet with the cards, ascci format, separated by a pipe.
    */
    RplethPacket handle_receive_cards(RplethPacket packet);

    /**
    * We received a message (on the BUS, from the wiegand reader we watch), that means a card was inserted.
    * We store all cards until we receive a RECEIVE_CARDS command.
    */
    void handle_wiegand_event();

    /**
    * List of cards pushed by SendCards Rpleth command.
    */
    std::list<std::string> cards_pushed_;

    /**
    * Cards our Wiegand reader read.
    */
    std::list<std::string> cards_read_;

    std::map<std::string, CircularBuffer> clients_;

    zmqpp::context &ctx_;

    /**
    * Stream socket to receive Rpleth connection
    */
    zmqpp::socket server_;

    /**
    * Subscribe to the message bus and listen for event sent by the wiegand reader we watch
    */
    zmqpp::socket bus_sub_;

    /**
    * Interface to the reader.
    */
    FWiegandReader *reader_;
};
