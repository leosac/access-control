#include "LEDModule.hpp"
#include "LedImpl.hpp"
#include "tools/log.hpp"

LEDModule::LEDModule(zmqpp::context &ctx,
        zmqpp::socket *pipe,
        boost::property_tree::ptree const &cfg) :
        BaseModule(ctx, pipe, cfg)
{
    process_config();
    for (auto &led : leds_)
    {
        reactor_.add(led->frontend(), std::bind(&LedImpl::handle_message, led.get()));
    }
}

int LEDModule::compute_timeout()
{
   std::chrono::system_clock::time_point tp = std::chrono::system_clock::time_point::max();

    for (auto &led : leds_)
    {
        if (led->next_update() < tp)
            tp = led->next_update();
    }
    return std::chrono::duration_cast<std::chrono::milliseconds>(tp - std::chrono::system_clock::now()).count();
}

void LEDModule::run()
{
    while (is_running_)
    {
        reactor_.poll(compute_timeout());
        for (auto &led : leds_)
        {
            if (led->next_update() < std::chrono::system_clock::now())
                led->update();
        }
    }
}

void LEDModule::process_config()
{
    boost::property_tree::ptree module_config = config_.get_child("module_config");

    for (auto &node : module_config.get_child("leds"))
    {
        boost::property_tree::ptree led_cfg = node.second;

        std::string led_name = led_cfg.get_child("name").data();
        std::string gpio_name = led_cfg.get_child("gpio").data();
        int default_blink_duration = led_cfg.get<int>("default_blink_duration", 1000);
        int default_blink_speed = led_cfg.get<int>("default_blink_speed", 200);

        LOG() << "Creating LED " << led_name << ", linked to GPIO" << gpio_name;

        leds_.push_back(std::shared_ptr<LedImpl>(new LedImpl(ctx_, led_name, gpio_name, default_blink_duration, default_blink_speed)));
    }
}
