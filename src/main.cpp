/**
 * \file main.cpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief standard main
 */

#include <iostream>

#include "exception/osacexception.hpp"
#include "dynlib/dynamiclibrary.hpp"
#include "thread/thread.hpp"

typedef union {
  void*		rawPtr;
  double	(*f)(double);
} Cast;

class JeanLouis : public IRunnable
{
public:
  void	run()
  {
    std::cout << "Salut, moi c'est JeanLouis !" << std::endl;
  }
};

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

    Thread	th;
    JeanLouis	jl;

    th.start(&jl);
    th.join();
  }
  catch (const OSACException& e)
  {
    std::cerr << e.what() << std::endl;
  }
  return (0);
}
