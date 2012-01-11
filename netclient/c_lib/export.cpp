#include "export.hpp"


#include <c_lib/animations/hitscan.hpp>

void animation_tick()
{   
    Animations::hitscanEffect_list.tick();
}

void animation_draw()
{
    Animations::hitscanEffect_list.draw();
}