#include "./c_lib/c_lib.cpp"
#include "./c_lib/main.cpp"
#include <c_lib/common/lua/lua.hpp>

#include <stdio.h>

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

    // set output unbuffered
    setvbuf(stdout, NULL, _IONBF, 0);   // NULL,0 are ignored when _IONBF enabled
    
    // program accepts one option: path to lua settings file
    // will use a default value if not provided
    if (argc > 1) LUA::set_options_file(argv[1]);

    Main::init();
    int ret = Main::run();
    return ret;
}
