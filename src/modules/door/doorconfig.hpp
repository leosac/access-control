/**
 * \file doorconfig.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief DoorConfig class declaration
 */

#ifndef DOORCONFIG_HPP
#define DOORCONFIG_HPP

#include "config/ixmlserializable.hpp"

class DoorConfig : public IXmlSerializable
{
    typedef struct {
        bool    open;
        int     start;
        int     end;
    } Day;

public:
    explicit DoorConfig() = default;
    ~DoorConfig() = default;

    DoorConfig(const DoorConfig& other) = delete;
    DoorConfig& operator=(const DoorConfig& other) = delete;

public:
    virtual void    serialize(ptree& node) override;
    virtual void    deserialize(const ptree& node) override;

public:
    bool    isDoorOpenable() const;

private:
    Day _days[7];
};

#endif // DOORCONFIG_HPP
