#include <tools/IVisitor.hpp>
#include "WiegandCard.hpp"
#include "BaseAuthSource.hpp"

using namespace Leosac::Auth;

WiegandCard::WiegandCard(const std::string &card_id, int bits) :
        id_(card_id),
        nb_bits_(bits)
{

}

void WiegandCard::accept(Leosac::Tools::IVisitor *visitor)
{
    visitor->visit(this);
}
