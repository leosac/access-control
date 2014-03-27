/**
 * \file main.cpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief standard main
 */

#include <iostream>

#include "Exception/OSACException.hpp"
#include "DynLib/DynamicLibrary.h"

typedef union {
  void*		rawPtr;
  double	(*f)(double);
} Cast;

int	main(int ac, char** av)
{
  static_cast<void>(ac);
  static_cast<void>(av);

  try
  {
    DynamicLibrary lib("libm.so");

    lib.open(DynamicLibrary::Now);

    Cast c;

    c.rawPtr = lib.getSymbol("cos");
    std::cout << c.f(3.1415f) << std::endl;

    lib.close();
  }
  catch (const OSACException& e)
  {
    std::cerr << e.what() << std::endl;
  }
  return (0);
}
