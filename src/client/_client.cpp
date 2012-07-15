// client.cpp : Defines the entry point for the console application.
//

#ifdef __GNUC__
#endif

#ifdef _MSC_VER
    #include "stdafx.h"
#endif

#include "_c_lib.cpp"
#include "_main.cpp"
#include <common/lua/lua.hpp>

#include <SDL.h>

//int main(int argc, char** argv)
#if __MSVC__
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif
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
    chdir("C:/dc_mmo/netclient");
    #endif
    
    // program accepts one option: path to lua settings file
    // will use a default value if not provided
    if (argc > 1 && argv[1][0] != '-') LUA::set_options_file((char*) argv[1]);

    Main::init(argc, argv);
    int ret = Main::run();
    return ret;
}
