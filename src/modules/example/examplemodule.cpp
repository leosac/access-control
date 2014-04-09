/**
 * \file examplemodule.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief example module class
 */

#include "examplemodule.hpp"

#include <iostream>

ExampleModule::ExampleModule() {}

ExampleModule::~ExampleModule() {}

ExampleModule::ExampleModule(const ExampleModule& /*other*/) {}

ExampleModule& ExampleModule::operator=(const ExampleModule& /*other*/)
{
    return (*this);
}

void ExampleModule::sendEvent(const Event& /*event*/)
{
    
}
