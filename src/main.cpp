/**
 * \file main.cpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief standard main
 */

#include <iostream>

#include "exception/osacexception.hpp"
#include "core.hpp"

int	main(int /*ac*/, char** /*av*/)
{
  try
  {
    Core    core;
  }
  catch (const OSACException& e)
  {
    std::cerr << e.what() << std::endl;
  }
  return (0);
}
