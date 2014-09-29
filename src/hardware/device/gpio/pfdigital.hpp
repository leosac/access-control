#include <list>
#include <map>
#include <thread>
#include "igpioprovider.hpp"

/**
* Provide access to GPIO managed by the PiFace Digital card.
*/
class PFDigital : public IGPIOProvider
{
public:
    void poll();

PFDigital();

    virtual GPIO *getGPIO(int gpioNo);

    virtual void registerListener(IGPIOListener *listener, GPIO *gpio);

    virtual void unregisterListener(IGPIOListener *listener, GPIO *gpio);

    void start_poll();
    void stop_poll();

private:
    void poll_loop();

    bool run_;
    std::thread             _pollThread;

    /**
    * Maps a GPIO pin number to a vector of listener;
    */
    std::map<int, std::list<IGPIOListener *>> listeners_;
};
