#include <c_lib/main.hpp>

//uncomment when compiling as shared lib
//#include <SDL.h>
//#include <c_lib/c_lib.cpp>

int main(int argc, char** argv)
{
    Main::init();
    int ret = Main::run();
    return ret;
}
