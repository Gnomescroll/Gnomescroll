#include <c_lib/c_lib.cpp>
#include <c_lib/main.hpp>
#include <c_lib/common/lua/lua.hpp>

int main(int argc, char** argv)
{
    // program accepts one option: path to lua settings file
    // will use a default value if not provided
    if (argc > 1) LUA::set_options_file(argv[1]);

    Main::init();
    int ret = Main::run();
    return ret;
}
