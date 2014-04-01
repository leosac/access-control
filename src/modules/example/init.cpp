#include "examplemoduleloader.hpp"

#ifndef EXPORT_PUBLIC
#   ifdef example_EXPORTS
#       define EXPORT_PUBLIC
#   else
#       define EXPORT_PUBLIC __declspec(dllimport)
#   endif
#endif

extern "C" IModuleLoader* EXPORT_PUBLIC getLoader()
{
    return (new ExampleModuleLoader);
}
