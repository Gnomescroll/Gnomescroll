//#include <SDL.h>
//#include <stdio.h>

#include <c_lib/main.hpp>

int main(int argc, char** argv)
{
    Main::init();
    int ret = Main::run();
    return ret;
}
