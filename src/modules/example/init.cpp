#include "examplemoduleloader.hpp"

#include <iostream> // FIXME Debug

#ifndef EXPORT_PUBLIC
#   ifdef example_EXPORTS
#       define EXPORT_PUBLIC __declspec(dllexport)
#   else
#       define EXPORT_PUBLIC
#   endif
#endif

extern "C" IModuleLoader* EXPORT_PUBLIC getLoader()
{
    return (new ExampleModuleLoader);
}
