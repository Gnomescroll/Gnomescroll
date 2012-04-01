#pragma once

#include <defines.h>

namespace Main
{

void init() GNOMESCROLL_API;
int run() GNOMESCROLL_API;

}

extern "C"
{
    void main_init() GNOMESCROLL_API;
    void main_init() { Main::init(); }
    int main_run() GNOMESCROLL_API;
    int main_run() { return Main::run(); }
}
