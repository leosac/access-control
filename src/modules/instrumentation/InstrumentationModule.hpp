#include <modules/BaseModule.hpp>

namespace Leosac
{
    namespace Module
    {
        /**
        * The instrumentation expose some internal of the program through IPC.
        *
        * It enable interesting testing features since it's goal is to emulate GPIO.
        */
        namespace Instrumentation
        {
            class InstrumentationModule : public BaseModule
            {
            public:

                InstrumentationModule(zmqpp::context &ctx,
                        zmqpp::socket *pipe,
                        const boost::property_tree::ptree &cfg);
                
                InstrumentationModule(const InstrumentationModule &) = delete;
                InstrumentationModule(InstrumentationModule &&) = delete;
                InstrumentationModule &operator=(const InstrumentationModule &) = delete;
                InstrumentationModule &operator=(InstrumentationModule &&) = delete;

            private:

                void handle_command();

                void handle_gpio_command(zmqpp::message *str);

                zmqpp::socket bus_push_;

                /**
                * IPC ROUTER socket.
                */
                zmqpp::socket controller_;
            };
        }
    }
}
