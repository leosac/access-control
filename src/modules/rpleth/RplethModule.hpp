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

    std::map<std::string, CircularBuffer> clients_;

    zmqpp::context &ctx_;

    /**
    * Stream socket to receive Rpleth connection
    */
    zmqpp::socket server_;

    /**
    * Interface to the reader.
    */
    FWiegandReader *reader_;
};
