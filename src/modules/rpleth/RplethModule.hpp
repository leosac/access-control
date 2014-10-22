#pragma once

#include "modules/BaseModule.hpp"
#include "hardware/device/FWiegandReader.hpp"
#include "rplethpacket.hpp"
#include <memory>
#include <vector>
#include <modules/rpleth/network/circularbuffer.hpp>

namespace Leosac
{
    namespace Module
    {
        /**
        * Namespace where implementation for Rpleth support takes place.
        * @see @ref mod_rpleth_main for end-user documentation.
        */
        namespace Rpleth
        {
            /**
            * Main class for the Rpleth module.
            * For configuration information, see @ref mod_rpleth_user_config.
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

                /**
                * Convert a card number from text to binary.
                */
                std::vector<uint8_t> card_convert_from_text(const std::string &s);

            private:
                void process_config();

                /**
                * Do we already know this client ?
                */
                bool client_connected(const std::string &identity) const;

                /**
                * Is the client in an invalid state ?
                * @return true if the client is in the `clients_failed_` list.
                */
                bool client_failed(const std::string &identity) const;

                static constexpr int buffer_size = 8192;

                /**
                * Try to handle a client message. This is called when we received any amount of data, for any client.
                * If there isn't enough data to handle a rpleth message, this does nothing.
                *
                * It returns true unless sending a packing would have blocked.
                *
                * @note There is a possible race between client disconnection and notification, so it sometime happens that
                * we attempt to send data to an already disconnected client. This would block forever so we use `dont_wait` flag
                * and report a failed client by returning false. Message from failed client are ignored until reception of
                * disconnection notification.
                */
                bool handle_client_msg(const std::string &client_identity, CircularBuffer &buf);

                /**
                * If we successfully built a packet, lets handle it.
                */
                RplethPacket handle_client_packet(RplethPacket packet);

                /**
                * Handle data available on server socket.
                */
                void handle_socket();

                /**
                * Handle Rpleth SendCards command: we will store the list of received card somewhere safe.
                * Cards are text data, separated by a pipe.
                */
                void rpleth_send_cards(const RplethPacket &packet);

                /**
                * Handle Rpleth ReceiveUnpresentedCards command.
                * This sends cards that were read to the Rpleth client.
                * @return a response packet with the cards, ascii format, separated by a pipe.
                */
                RplethPacket rpleth_receive_cards(const RplethPacket &packet);

                /**
                * Handle Rpleth Beep command.
                */
                void rpleth_beep(const RplethPacket &p);

                /**
                * Handle rpleth greenled command.
                */
                void rpleth_greenled(const RplethPacket &p);

                /**
                * Flush the cards_read_stream_ list to clients.
                * This will notify client of all card that we read.
                */
                void rpleth_publish_card();

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
                * Valid cards our Wiegand reader read: cards that were not pushed are not stored here.
                */
                std::list<std::string> cards_read_;

                /**
                * If stream mode is on, all cards read are stored here.
                */
                std::list<std::string> cards_read_stream_;

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

                /**
                * Client that are "failed".
                * A client is considered fail if a `send()` to them would have block.
                * It's likely they are disconnected but we dont know it yet. Message from those client are ignored.
                */
                std::vector<std::string> failed_clients_;

                bool stream_mode_;
            };

        }
    }
}