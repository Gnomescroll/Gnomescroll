#if _MSC_VER
# include "stdafx.h" 
#endif

#ifdef __MSVC__
# include <direct.h>
# define getcwd _getcwd
# define chdir _chdir
# include <windows.h> 
# include <winbase.h> // usleep function
# include <tchar.h>

# include <stdio.h>
# include <stdlib.h>
# include <stdint.h>

# include <string.h>
# include <math.h>
#else
# include <unistd.h>
#endif

#include "_c_lib.cpp"
#include "_main.cpp"
#include <common/lua/lua.hpp>

int main(int argc, char* argv[])
{
    #if PRODUCTION
    printf("Production build\n");
    #else
    printf("Debug build\n");
    #endif

    printf("Server version: %d\n", GS_VERSION);

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
    if (argc > 1 && argv[1][0] != '-') LUA::set_options_file(argv[1]);

    Main::init(argc, argv);
    int ret = Main::run();

    return ret;
}
