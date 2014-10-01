/**
* An abstract implementation of a GPIO pin.
*
* The goal of this abstraction is to allow use of multiple "gpio backend".
*     1. The `/sys/class/gpio` kernel api, those are classic GPIO usage.
*     2. Through the PifaceDigital extension card. It uses the SPI bus internally.
*/
class AGPIO
{
public:
    bool getValue();
    void setValue(bool v);
};