#include "_c_lib.cpp"
#include "_main.cpp"
#include <common/lua/lua.hpp>

#include <SDL.h>

int main(int argc, char* argv[])
{
    #if PRODUCTION
    printf("Production build\n");
    #else
    printf("Debug build\n");
    #endif

    printf("Client version: %d\n", GS_VERSION);

    #ifdef __APPLE__
    set_working_directory_to_Resources();
    #endif

    #ifdef __MSVC__
    chdir("C:/dc_mmo/netclient");
    #endif

    // program accepts one option: path to lua settings file
    // will use a default value if not provided
    if (argc > 1 && argv[1][0] != '-') LUA::set_options_file(argv[1]);

    Main::init(argc, const_cast<const char**>(argv));
    if (!input_state.quit && !signal_exit && !_quit)
        return Main::run();
    return 0;
}
