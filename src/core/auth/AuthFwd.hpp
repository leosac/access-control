#pragma once

#include <memory>

namespace Leosac
{
namespace Auth
{
class IUser;
using IUserPtr = std::shared_ptr<IUser>;
using UserId   = unsigned long;

class IAccessProfile;
using IAccessProfilePtr = std::shared_ptr<IAccessProfile>;

class CredentialValidity;

class AuthTarget;
using AuthTargetPtr = std::shared_ptr<AuthTarget>;

class IAuthenticationSource;
using IAuthenticationSourcePtr = std::shared_ptr<IAuthenticationSource>;

class BaseAuthSource;
using BaseAuthSourcePtr = std::shared_ptr<BaseAuthSource>;

class WiegandCard;
}
}
