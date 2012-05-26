
#include <c_lib.cpp>
#include <main.cpp>
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
 
