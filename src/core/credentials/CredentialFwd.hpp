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

#include <memory>
#include <odb/lazy-ptr.hxx>

namespace Leosac
{
namespace Cred
{

// Credentials
class ICredential;
using ICredentialPtr = std::shared_ptr<ICredential>;

class Credential;
using CredentialId    = unsigned long;
using CredentialPtr   = std::shared_ptr<Credential>;
using CredentialLWPtr = odb::lazy_weak_ptr<Credential>;
using CredentialLPtr  = odb::lazy_shared_ptr<Credential>;

class IRFIDCard;
using IRFIDCardPtr = std::shared_ptr<IRFIDCard>;
class RFIDCard;
using RFIDCardPtr = std::shared_ptr<RFIDCard>;

class IPinCode;
class PinCode;
using PinCodePtr = std::shared_ptr<PinCode>;

class RFIDCardPin;
using RFIDCardPinPtr = std::shared_ptr<RFIDCardPin>;
}
}
