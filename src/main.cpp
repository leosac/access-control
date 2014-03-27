/**
 * \file main.cpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief standard main
 */

#include <list>
#include <iostream>

int	main(int ac, char** av)
{
  static_cast<void>(ac);
  static_cast<void>(av);

  std::list<int>	list;

  list.push_front(42);

  for (auto it : list)
  {
    std::cout << it << std::endl;
  }

  return (0);
}
