#include "gpio.hpp"

class PFDigitalGPIO : public GPIO
{
public:
    virtual int getPinNo() const override;

    virtual const std::string &getPath() const override;

    virtual int getPollFd() const override;

    virtual Direction getDirection() const override;

    virtual void setDirection(Direction direction) const override;

    virtual void setValue(Value value) const override;

    virtual bool isActiveLow() const override;

    virtual void setActiveLow(bool state) const override;

    virtual bool hasInterruptsSupport() const override;

    virtual EdgeMode getEdgeMode() const override;

    virtual void setEdgeMode(EdgeMode mode) const override;

    PFDigitalGPIO(int pinNo);
    virtual bool        getValue() const override;
    virtual void        setValue(bool value) const override;
};