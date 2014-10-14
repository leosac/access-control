#include "FWiegandReader.hpp"

FWiegandReader::FWiegandReader(zmqpp::context &ctx,
        std::string const &reader_name) :
        backend_(ctx, zmqpp::socket_type::req)
{
    backend_.connect("inproc://" + reader_name);
}

bool FWiegandReader::greenLedOn()
{
    zmqpp::message msg;
    msg << "GREEN_LED" << "ON";

    return send_to_backend(msg);
}

bool FWiegandReader::greenLedOff()
{
    zmqpp::message msg;
    msg << "GREEN_LED" << "OFF";

    return send_to_backend(msg);
}

bool FWiegandReader::greenLedBlink(int duration, int speed)
{
    zmqpp::message msg;
    msg << "GREEN_LED" << "BLINK" << std::to_string(duration) << std::to_string(speed);

    return send_to_backend(msg);
}

bool FWiegandReader::send_to_backend(zmqpp::message &msg)
{
    std::string rep;
    backend_.send(msg);
    backend_.receive(rep);

    assert(rep == "OK" || rep == "KO");
    if (rep == "OK")
        return true;
    return false;
}
