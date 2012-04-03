#include <c_lib/main.hpp>
#include <SDL.h>

//uncomment when not compiling as shared lib
#include <SDL.h>
//#include <c_lib/c_lib.cpp>

int main(int argc, char** argv)
{
    //Main::init();
    //int ret = Main::run();
    main_init();
    int ret = main_run();
    return ret;
}
