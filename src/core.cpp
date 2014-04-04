/**
 * \file core.cpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief module managing class
 */

#include "core.hpp"

#include <thread>
#include <iostream> // FIXME Debug

#include "osac.hpp"
#include "signal/signalhandler.hpp"
#include "exception/signalexception.hpp"
#include "modules/journallogger.hpp"
#include "modules/imoduleloader.hpp"
#include "hardware/hwmanager.hpp"
#include "hardware/device/gpio.hpp" // FIXME Debug
#include "dynlib/dynamiclibrary.hpp"

Core::Core()
:   _isRunning(false),
    _hwManager(nullptr)
{}

Core::~Core() {}

Core::Core(const Core& /*other*/) {}

Core& Core::operator=(const Core& /*other*/)
{
    return (*this);
}

void Core::handleSignal(int /*signal*/)
{
    std::lock_guard<std::mutex> lg(_runMutex);
    if (_isRunning)
    {
        dispatchEvent(Event("caught signal", "Core"));
        _isRunning = false;
    }
}

void Core::run(const std::list<std::string>& args)
{
    _args = args;
    if (!parseArguments())
        return;
    load();
    dispatchEvent(Event("starting", "Core"));
    _runMutex.lock();
    _isRunning = true;
    while (_isRunning)
    {
        _runMutex.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        dispatchEvent(Event("alive", "Core"));
        _runMutex.lock();
    }
    _runMutex.unlock();
    dispatchEvent(Event("exiting", "Core"));
    unload();
}

bool Core::parseArguments()
{
    for (auto arg : _args)
    {
        if (arg == "--version")
        {
            std::cout << "Version: " << OSAC::getVersionString() << std::endl;
            return (false);
        }
    }
    return (true);
}

void Core::load()
{
    DynamicLibrary          df("./modules/example/libexample.so");
    IModuleLoader::InitFunc func;
    IModuleLoader*          moduleloader;
    IModule*                module;

    df.open();
    void* s = df.getSymbol("getLoader");
    *reinterpret_cast<void**>(&func) = s;

    moduleloader = func();
    std::cout << "Module " << moduleloader->getModuleName() << " loaded (v" << moduleloader->getVersionString() << ")" << std::endl;

    module = moduleloader->instanciateModule();
    module->sayHello();
    delete moduleloader;
    delete module;
    df.close();

    _hwManager = new HWManager;

    if (!_args.size())
        throw (SignalException("Not enough arguments"));

    GPIO* gpio = _hwManager->reserveGPIO(std::stoi(_args.front()));

    std::cout << "GPIO No" << gpio->getPinNo() << " reserved" << std::endl;

    gpio->setActiveLow(true);
    std::cout << "IsActiveLow=" << gpio->isActiveLow() << std::endl;
    gpio->setActiveLow(false);
    std::cout << "IsActiveLow=" << gpio->isActiveLow() << std::endl;

    gpio->setDirection(GPIO::In);
    std::cout << "DirectionIsIn=" << (gpio->getDirection() == GPIO::In) << std::endl;
    gpio->setEdgeMode(GPIO::Rising); // Won't go further

    while (1)
    {
        std::cout << "Boucle" << std::endl;
        gpio->setValue(false);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        gpio->setValue(true);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    gpio->setEdgeMode(GPIO::Falling);

    _loggerModules.push_front(new JournalLogger(Event::Debug));

    try
    {
        SignalHandler::registerCallback(this);
    }
    catch (const SignalException& e)
    {
        dispatchEvent(Event(e.what(), "Core"));
    }
}

void Core::unload()
{
    for (auto logger : _loggerModules)
        delete logger;
    _loggerModules.clear();

    delete _hwManager;
    _hwManager = nullptr;
}

void Core::dispatchEvent(const Event& event)
{
    for (auto logger : _loggerModules)
        logger->sendEvent(event);
}
