#pragma once
#include <list>
#include <map>
#include <thread>
#include <mutex>
#include <queue>
#include "igpioprovider.hpp"

/**
* Provide access to GPIO managed by the PiFace Digital card.
*/
class PFDigital : public IGPIOProvider
{
public:
    void poll();

PFDigital();
    ~PFDigital();

    virtual GPIO *getGPIO(int gpioNo);

    virtual void registerListener(IGPIOListener *listener, GPIO *gpio);

    virtual void unregisterListener(IGPIOListener *listener, GPIO *gpio);

    void start_poll();
    void stop_poll();

    /**
    * Set the output PIN (starting from 0) to on or off.
    * This method is thread safe, and we assume it is called from
    * an other thread.
    * It will queue the "toggle on/off" order, and write to a pipe
    * to unlock the pfdigital provider.
    */
    void set_pin_value(int pinNo, bool v);

private:

    void poll_loop();


    /**
    * Since write to a GPIO are asynchronous, this flush pending write.
    * This is done after each polliong phase.
    */
    void flush_write();

    bool run_;
    std::thread             _pollThread;
    std::mutex              order_queue_lock;

    /**
    * The "write" GPIO value order queue.
    * Since we cant use libpifacedigital from multiple thread, we queue those write orders.
    *
    * The pair is (gpioNo, value).
    */
    std::queue<std::pair<int, bool>> order_queue;

    /**
    * Maps a GPIO pin number to a vector of listener;
    */
    std::map<int, std::list<IGPIOListener *>> listeners_;

    int piface_fd_;
};
