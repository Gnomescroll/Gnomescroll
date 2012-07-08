// client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#ifdef __GNUC__
#endif

#ifdef __MSVC__
#endif

#include "_c_lib.cpp"
#include "_main.cpp"
#include <common/lua/lua.hpp>

#include <SDL.h>

//int main(int argc, char** argv)
int _tmain(int argc, _TCHAR* argv[])
{
    #if PRODUCTION
    printf("Production build\n");
    #else
    printf("Debug build\n");
    #endif

    #ifdef __APPLE__
    set_working_directory_to_Resources();
    #endif

    #ifdef __MSVC__
    chdir("C:/msvc/client");
    #endif
    
    // program accepts one option: path to lua settings file
    // will use a default value if not provided
    if (argc > 1) LUA::set_options_file((char*) argv[1]);

    Main::init();
    int ret = Main::run();
    return ret;
}