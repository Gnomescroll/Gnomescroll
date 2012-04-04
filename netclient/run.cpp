
#include <c_lib/c_lib.cpp>
#include <c_lib/main.hpp>

#include <SDL.h>

int main(int argc, char** argv)
{
    
    Main::init();
    int ret = Main::run();
    return ret;
}
