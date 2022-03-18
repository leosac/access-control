/*
    Copyright (C) 2014-2022 Leosac

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

#include "hardware/ExternalMessage.hpp"

namespace Leosac
{
namespace Hardware
{

ExternalMessage::ExternalMessage()
    : Device(DeviceClass::EXTERNAL_MESSAGE),
    subject_(""),
    direction_(Hardware::ExternalMessage::Direction::Subscribe),
    virtualtype_(Hardware::DeviceClass::GPIO)
{
}

std::string ExternalMessage::subject() const
{
  return subject_;
}

void ExternalMessage::subject(const std::string& subject)
{
  subject_ = subject;
}

ExternalMessage::Direction ExternalMessage::direction() const
{
  return direction_;
}

void ExternalMessage::direction(ExternalMessage::Direction direction)
{
  direction_ = direction;
}

DeviceClass ExternalMessage::virtualtype() const
{
  return virtualtype_;
}

void ExternalMessage::virtualtype(DeviceClass virtualtype)
{
  virtualtype_ = virtualtype;
}

std::string ExternalMessage::payload() const
{
  return payload_;
}

void ExternalMessage::payload(const std::string& payload)
{
  payload_ = payload;
}

void ExternalMessage::validation_callback(odb::callback_event e, odb::database &db) const
{
  Device::validation_callback(e, db);
}
}
}
