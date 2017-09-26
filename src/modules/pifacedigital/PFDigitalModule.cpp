/*
    Copyright (C) 2014-2016 Leosac

    This file is part of Leosac.

    Leosac is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leosac is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "PFDigitalModule.hpp"
#include "PFGPIO.hpp"
#include "core/CoreUtils.hpp"
#include "core/GetServiceRegistry.hpp"
#include "exception/EntityNotFound.hpp"
#include "exception/ModelException.hpp"
#include "exception/gpioexception.hpp"
#include "hardware/FGPIO.hpp"
#include "hardware/GPIO_odb.h"
#include "mcp23s17.h"
#include "modules/pifacedigital/CRUDHandler.hpp"
#include "modules/pifacedigital/PFGPIO_odb.h"
#include "modules/websock-api/Messages.hpp"
#include "modules/websock-api/Service.hpp"
#include "pifacedigital.h"
#include "tools/AssertCast.hpp"
#include "tools/db/DBService.hpp"
#include "tools/enforce.hpp"
#include "tools/log.hpp"
#include "tools/timeout.hpp"
#include <boost/iterator/transform_iterator.hpp>
#include <fcntl.h>
#include <odb/schema-catalog.hxx>
#include <thread>

namespace Leosac
{
namespace Module
{
namespace Piface
{

PFDigitalModule::PFDigitalModule(zmqpp::context &ctx,
                                 zmqpp::socket *module_manager_pipe,
                                 const boost::property_tree::ptree &config,
                                 CoreUtilsPtr utils)
    : BaseModule(ctx, module_manager_pipe, config, utils)
    , bus_push_(ctx_, zmqpp::socket_type::push)
    , helper_thread_(ctx_, utils->config_checker())
    , degraded_mode_(false)
{
    if (pifacedigital_open(0) == -1)
    {
        // Failed to init piface device. Run module in degraded mode (only WS api)
        degraded_mode_ = true;
        ERROR("Cannot open PifaceDigital device. Are you running on device with SPI "
              "bus and have SPI linux kernel module enabled ?");
        process_config();
        return;
    }
    int ret = pifacedigital_enable_interrupts();
    ASSERT_LOG(ret == 0, "Failed to enable interrupt on piface board");

    process_config();
    bus_push_.connect("inproc://zmq-bus-pull");
    for (auto &gpio : gpios_)
    {
        reactor_.add(gpio.sock_, std::bind(&PFDigitalPin::handle_message, &gpio));
    }

    std::string path_to_gpio =
        "/sys/class/gpio/gpio" + std::to_string(GPIO_INTERRUPT_PIN) + "/value";
    interrupt_fd_ = open(path_to_gpio.c_str(), O_RDONLY | O_NONBLOCK);
    LEOSAC_ENFORCE(interrupt_fd_ > 0, "Failed to open GPIO file");
    pifacedigital_read_reg(0x11, 0); // flush

    // Somehow it was required poll with "poll_pri" and "poll_error". It used to
    // work with poll_pri alone before. Need to investigate more. todo !
    reactor_.add(interrupt_fd_, std::bind(&PFDigitalModule::handle_interrupt, this),
                 zmqpp::poller::poll_pri | zmqpp::poller::poll_error);
}

void PFDigitalModule::run()
{
    while (is_running_)
    {
        auto itr_transform =
            [](const PFDigitalPin &p) -> std::chrono::system_clock::time_point {
            return p.next_update();
        };

        auto timeout = Tools::compute_timeout(
            boost::make_transform_iterator(gpios_.begin(), itr_transform),
            boost::make_transform_iterator(gpios_.end(), itr_transform));
        reactor_.poll(timeout);
        for (auto &gpio_pin : gpios_)
        {
            if (gpio_pin.next_update() < std::chrono::system_clock::now())
                gpio_pin.update();
        }
    }
    remove_ws_handlers();
}

void PFDigitalModule::handle_interrupt()
{
    // get interrupt state.
    std::array<char, 64> buffer{};
    ssize_t ret;

    ret = ::read(interrupt_fd_, &buffer[0], buffer.size());
    ASSERT_LOG(ret >= 0,
               "Reading on interrupt_fd gave unexpected return value: " << ret);
    ret = ::lseek(interrupt_fd_, 0, SEEK_SET);
    ASSERT_LOG(ret >= 0,
               "Lseeking on interrupt_fd gave unexpected return value: " << ret);

    uint8_t states = pifacedigital_read_reg(0x11, 0);
    for (int i = 0; i < 8; ++i)
    {
        if (((states >> i) & 0x01) == 0)
        {
            // signal interrupt if needed (ie the pin is registered in config)
            std::string gpio_name;
            if (get_input_pin_name(gpio_name, i))
            {
                bus_push_.send(zmqpp::message()
                               << std::string("S_INT:" + gpio_name));
            }
        }
    }
}

bool PFDigitalModule::get_input_pin_name(std::string &dest, int idx)
{
    for (auto &gpio : gpios_)
    {
        if (gpio.gpio_no_ == idx && gpio.direction_ == PFDigitalPin::Direction::In)
        {
            dest = gpio.name_;
            return true;
        }
    }
    return false;
}

void PFDigitalModule::process_xml_config(const boost::property_tree::ptree &cfg)
{
    boost::property_tree::ptree module_config = cfg.get_child("module_config");

    for (auto &node : module_config.get_child("gpios"))
    {
        boost::property_tree::ptree gpio_cfg = node.second;

        std::string gpio_name      = gpio_cfg.get_child("name").data();
        int gpio_no                = std::stoi(gpio_cfg.get_child("no").data());
        std::string gpio_direction = gpio_cfg.get_child("direction").data();
        bool gpio_value            = gpio_cfg.get<bool>("value", false);

        INFO("Creating GPIO " << gpio_name << ", with no " << gpio_no
                              << ". direction = " << gpio_direction);

        PFDigitalPin pin(ctx_, gpio_name, gpio_no,
                         gpio_direction == "in" ? PFDigitalPin::Direction::In
                                                : PFDigitalPin::Direction::Out,
                         gpio_value);

        if (gpio_direction != "in" && gpio_direction != "out")
            throw GpioException("Direction (" + gpio_direction + ") is invalid");
        gpios_.push_back(std::move(pin));
        utils_->config_checker().register_object(gpio_name,
                                                 ConfigChecker::ObjectType::GPIO);
    }
}

void PFDigitalModule::process_config()
{
    bool use_db = config_.get<bool>("module_config.use_database", false);
    if (!use_db && degraded_mode_)
    {
        throw LEOSACException("We failed to open piface digital device and wont "
                              "enable database support. There is nothing to do but "
                              "exit.");
    }
    else if (!use_db)
    {
        // Process XML based config.
        process_xml_config(config_);
    }
    else
    {
        // Database enabled configuration.
        setup_database();

        WSHelperThread::ModuleParameters parameters{};
        parameters.degraded_mode = degraded_mode_;
        helper_thread_.set_parameter(parameters);

        helper_thread_.register_ws_handlers();

        load_config_from_database();
    }
}

void PFDigitalModule::remove_ws_handlers()
{
    auto ws_service = get_service_registry().get_service<WebSockAPI::Service>();
    if (ws_service)
    {
        ws_service->unregister_handler("pfdigital.is_degraded_mode");
        ws_service->unregister_handler("pfdigital.test_output_pin");

        // Remove 4 handlers, one for each CRUD operation
        ws_service->unregister_handler("pfdigital.gpio.create");
        ws_service->unregister_handler("pfdigital.gpio.read");
        ws_service->unregister_handler("pfdigital.gpio.update");
        ws_service->unregister_handler("pfdigital.gpio.delete");
    }
}

void PFDigitalModule::setup_database()
{
    using namespace odb;
    using namespace odb::core;
    auto db          = utils_->database();
    schema_version v = db->schema_version("module_pifacedigital");
    schema_version cv(schema_catalog::current_version(*db, "module_pifacedigital"));
    if (v == 0)
    {
        transaction t(db->begin());
        schema_catalog::create_schema(*db, "module_pifacedigital");
        t.commit();
    }
    else if (v < cv)
    {
        INFO("PIFACEDIGITAL_GPIO Module performing database migration. Going from "
             "version "
             << v << " to version " << cv);
        transaction t(db->begin());
        schema_catalog::migrate(*db, cv, "module_pifacedigital");
        t.commit();
    }
}

void PFDigitalModule::load_config_from_database()
{
    using Result = odb::result<PFGPIO>;
    DBPtr db     = utils_->database();
    odb::transaction t(db->begin());
    Result result = db->query<PFGPIO>();

    for (const auto &gpio : result)
    {
        // For each PFGPIO object in the database, create a PFDigitalPin

        INFO("Creating GPIO "
             << gpio.name() << ", with no " << gpio.number() << ". direction = "
             << (gpio.direction() == PFDigitalPin::Direction::In ? "in" : "out"));
        PFDigitalPin pin(ctx_, gpio.name(), gpio.number(), gpio.direction(),
                         gpio.default_value());
        gpios_.push_back(std::move(pin));
        utils_->config_checker().register_object(gpio.name(),
                                                 ConfigChecker::ObjectType::GPIO);
    }

    t.commit();
}

void WSHelperThread::run_io_service()
{
    work_ = std::make_unique<boost::asio::io_service::work>(io_);
    io_.run();
}

void WSHelperThread::on_service_event(const service_event::Event &e)
{
    if (e.type() == service_event::REGISTERED)
    {
        auto &service_registered_event =
            assert_cast<const service_event::ServiceRegistered &>(e);
        if (service_registered_event.interface_type() ==
            boost::typeindex::type_id<WebSockAPI::Service>())
        {
            // request registration of handler from our helper thread.
            io_.post([this]() { register_ws_handlers(); });
        }
    }
}

void WSHelperThread::register_ws_handlers()
{
    auto ws_service = get_service_registry().get_service<WebSockAPI::Service>();
    if (ws_service)
    {
        std::lock_guard<decltype(mutex_)> lg(mutex_);
        ws_service->register_handler(
            [mode = parameters_.degraded_mode](const WebSockAPI::RequestContext rc) {
                json j{{"mode", mode}};
                return j;
            },
            "pfdigital.is_degraded_mode");

        ws_service->register_handler(
            [this](const WebSockAPI::RequestContext rc) {
                rc.security_ctx.enforce_permission(SecurityContext::Action::IS_ADMIN,
                                                   {});

                int gpio_id = rc.original_msg.content.at("gpio_id").get<int>();
                this->test_output_pin(gpio_id);
                return json{};
            },
            "pfdigital.test_output_pin");

        ws_service->register_crud_handler("pfdigital.gpio",
                                          &CRUDHandler::instanciate);
    }
    else
    {
        service_event_listener_ = get_service_registry().register_event_listener(
            [this](const service_event::Event &e) { on_service_event(e); });
    }
}

void WSHelperThread::set_parameter(WSHelperThread::ModuleParameters param)
{
    std::lock_guard<decltype(mutex_)> lg(mutex_);
    parameters_ = param;
}

void WSHelperThread::test_output_pin(int gpio_id)
{
    // We want to make the target output pin blink.
    // We must first make sure a few assumptions hold:
    //    + The GPIO exists in our database (so we can retrieve its name)
    //    + The GPIO is an output GPIO.
    //    + The GPIO exists in the ConfigChecker (ie, the server
    //      up-to-date with regards to what the database stores).

    DBPtr db = get_service_registry().get_service<DBService>()->db();
    odb::transaction t(db->begin());
    auto gpio = db->find<PFGPIO>(gpio_id);
    t.commit();

    if (!gpio)
        throw EntityNotFound(gpio_id, "pfdigital.gpio");

    if (gpio->direction() != Hardware::GPIO::Direction::Out)
        throw ModelException("", "GPIO is not an output GPIO.");

    if (!config_checker_.has_object(gpio->name(), ConfigChecker::ObjectType::GPIO))
    {
        throw ModelException(
            "", "GPIO doesn't exist in the runtime configuration checker. "
                "This probably means that you need to restart Leosac to load the "
                "new configuration");
    }

    // We have to use the ZMQ based messaging infrastructure.
    Hardware::FGPIO gpio_facade(zmq_context_, gpio->name());
    // This will block the Piface GPIO module from responding
    // to WS message. This is fine for now...
    for (int i = 0; i < 8; ++i)
    {
        gpio_facade.toggle();
        std::this_thread::sleep_for(std::chrono::milliseconds(750));
    }
}
}
}
}
