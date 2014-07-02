/**
 * \file authenticator.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Authenticator class implementation
 */

#include "authenticator.hpp"

#include <sstream>
#include <iomanip>

void Authenticator::serialize(ptree& node)
{
    ptree& child = node.add("auth", std::string());

    child.put("user", "test"); // FIXME
    for (const auto& csn : _csnList)
    {
        ptree ptre;

        ptre.put(std::string(), serializeCard(csn));
        child.add_child("card", ptre);
    }
    _csnList.clear();
}

void Authenticator::deserialize(const ptree& node)
{
    _csnList.clear();
    for (const auto& v : node.get_child("auth"))
    {
        if (v.first == "card")
            authorizeCard(deserializeCard(v.second.data()));
    }
}

bool Authenticator::hasAccess(const CSN& csn)
{
    for (const auto& c : _csnList)
    {
        bool match = true;
        if (c.size() != csn.size())
            continue;
        for (std::size_t i = 0; i < c.size(); ++i)
        {
            if (csn[i] != c[i])
            {
                match = false;
                break;
            }
        }
        if (match)
            return (true);
    }
    return (false);
}

void Authenticator::authorizeCard(const Authenticator::CSN& csn)
{
    _csnList.push_back(csn);
}

std::string Authenticator::serializeCard(const Authenticator::CSN& csn)
{
    std::ostringstream  oss;

    for (std::size_t i = 0; i < csn.size(); ++i)
    {
        if (i > 0)
            oss << ':';
        oss << std::setw(2) << std::setfill('0') << std::hex << static_cast<unsigned>(csn[i]);
    }
    return (oss.str());
}

Authenticator::CSN Authenticator::deserializeCard(const std::string& card)
{
    std::string         csnString(card);
    std::istringstream  iss;
    CSN                 csn;
    unsigned int        byte;

    std::replace(csnString.begin(), csnString.end(), ':', ' ');
    iss.str(csnString);
    while (iss >> std::hex >> byte)
        csn.push_back(byte);
    return (csn);
}
