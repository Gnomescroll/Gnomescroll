#include "export.hpp"


//#include <c_lib/animations/hitscan.hpp>
#include <c_lib/state/client_state.hpp>

void animation_tick()
{   
    ClientState::hitscan_effect_list.tick();
}

void animation_draw()
{
    ClientState::hitscan_effect_list.draw();
}
