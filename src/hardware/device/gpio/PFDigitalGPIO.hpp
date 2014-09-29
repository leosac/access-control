#include "gpio.hpp"
#include "pfdigital.hpp"

/**
* A GPIO PIN, when provided by PifaceDigital.
* By convention, pin 0 to 7 are READ ONLY pin. 8-15 are WRITE ONLY. We use this to determine direction.
*/
class PFDigitalGPIO : public GPIO
{
public:
    virtual int getPinNo() const override;

    virtual const std::string &getPath() const override;

    virtual int getPollFd() const override;

    virtual Direction getDirection() const override;

    /**
    * Ignored, direction is determined from the pinNO.
    */
    virtual void setDirection(Direction direction) const override;

    virtual void setValue(Value value) const override;

    /**
    * Always returns false.
    */
    virtual bool isActiveLow() const override;

    /**
    * Do nothing. Ignored.
    */
    virtual void setActiveLow(bool state) const override;

    virtual bool hasInterruptsSupport() const override;

    virtual EdgeMode getEdgeMode() const override;

    virtual void setEdgeMode(EdgeMode mode) const override;

    PFDigitalGPIO(int pinNo, PFDigital *provider);
    virtual bool        getValue() const override;
    virtual void        setValue(bool value) const override;

private:
    PFDigital *pfdigital_provider_;
};