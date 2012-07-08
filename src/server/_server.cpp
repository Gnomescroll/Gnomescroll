#ifdef __GNUC__
#endif

//#define __MSVC__

#include "stdafx.h" //precompiled headers

/*
#define __MSVC__

#ifdef __MSVC__

    #define DC_SERVER 1
    
    #define __attribute__(x)
    #define __attribute(x)
    #define __WIN32__
    //#define __MSVC__

    #pragma warning(disable: 4244) // possible loss of data

    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX

    #define _CRT_SECURE_NO_DEPRECATE
    #define _CRT_SECURE_NO_WARNINGS

    //

    #include <windows.h> 
    #include <winbase.h> // usleep function

#endif
*/

#include "_c_lib.cpp"
#include "_main.cpp"
#include <common/lua/lua.hpp>

#include <stdio.h>

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
    chdir("C:/dc_mmo/server");
#endif
    // set output unbuffered
    setvbuf(stdout, NULL, _IONBF, 0);   // NULL,0 are ignored when _IONBF enabled
    
    // program accepts one option: path to lua settings file
    // will use a default value if not provided
    if (argc > 1) LUA::set_options_file( (char*) argv[1] );

    Main::init();
    int ret = Main::run();


    //Sleep(10000);
    return ret;
}