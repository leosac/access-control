#include "WiegandCard.hpp"

using namespace Leosac::Auth;

WiegandCard::WiegandCard(const std::string &card_id, int bits) :
        id_(card_id),
        nb_bits_(bits)
{

}
