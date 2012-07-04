

#ifdef __GNUC__
#endif

#if __MSVC__
    #define __attribute__(x)
    #define __attribute(x)
    #define __WIN32__
    #define __MSVC__

    #pragma warning(disable: 4244) // possible loss of data

    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX

    #define _CRT_SECURE_NO_DEPRECATE
    #define _CRT_SECURE_NO_WARNINGS
#endif

#include "c_lib.cpp"
#include "main.cpp"
#include <common/lua/lua.hpp>

#include <SDL.h>

int main(int argc, char** argv)
{
    #if PRODUCTION
    printf("Production build\n");
    #else
    printf("Debug build\n");
    #endif

    #ifdef __APPLE__
    set_working_directory_to_Resources();
    #endif
    
    // program accepts one option: path to lua settings file
    // will use a default value if not provided
    if (argc > 1) LUA::set_options_file(argv[1]);

    Main::init();
    int ret = Main::run();
    return ret;
}
 
