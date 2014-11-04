#include "FWiegandReader.hpp"

using namespace Leosac::Hardware;

FWiegandReader::FWiegandReader(zmqpp::context &ctx,
        std::string const &reader_name) :
        backend_(ctx, zmqpp::socket_type::req),
        name_(reader_name)
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

bool FWiegandReader::beep(int duration)
{
    zmqpp::message msg;
    msg << "BEEP" << std::to_string(duration);

    return send_to_backend(msg);
}

bool FWiegandReader::buzzerOn()
{
    zmqpp::message msg;
    msg << "BEEP_ON";

    return send_to_backend(msg);
}

bool FWiegandReader::buzzerOff()
{
    zmqpp::message msg;
    msg << "BEEP_OFF";

    return send_to_backend(msg);
}

const std::string &FWiegandReader::name() const
{
    return name_;
}
