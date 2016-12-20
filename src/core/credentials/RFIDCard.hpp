/*
    Copyright (C) 2014-2016 Leosac

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

#pragma once

#include "core/credentials/Credential.hpp"
#include "core/credentials/IRFIDCard.hpp"

namespace Leosac
{
namespace Cred
{
/**
 * An RFID card credential.
 */
#pragma db object polymorphic optimistic
class RFIDCard : public virtual IRFIDCard, public Credential
{
  public:
    RFIDCard() = default;
    virtual const std::string &card_id() const override;

    virtual int nb_bits() const override;

    virtual uint64_t to_int() const override;

    virtual uint64_t to_raw_int() const override;

    void nb_bits(int i) override;

    void card_id(const std::string &string) override;

  protected:
    int nb_bits_;
    std::string card_id_;

  private:
    /**
     * Extract the card ID, assuming the format to be Wiegand26.
     */
    uint64_t to_wiegand_26() const;

    /**
     * Extract the card ID, assuming the format to be Wiegand34.
     */
    uint64_t to_wiegand_34() const;

    friend class odb::access;
};

class RFIDCardValidator
{
  public:
    static void validate(const IRFIDCard &card);
    static void validate_card_id(const std::string &card_id);
    static void validate_nb_bits(int nb_bits);
};
}
}
