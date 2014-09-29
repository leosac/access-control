#include <list>
#include <map>
#include "igpioprovider.hpp"

/**
* Provide access to GPIO managed by the PiFace Digital card.
*
* This could inherit from IGPIOProvide, but we'll that out for now, at least.
*/
class PFDigital : public IGPIOProvider
{
public:
    void poll();


    virtual GPIO *getGPIO(int gpioNo);

    virtual void registerListener(IGPIOListener *listener, GPIO *gpio);

    virtual void unregisterListener(IGPIOListener *listener, GPIO *gpio);

private:
    /**
    * Maps a GPIO pin number to a vector of listener;
    */
    std::map<int, std::list<IGPIOListener *>> listeners_;
};
