#pragma once

namespace t_mob
{

#if GS_ASSIMP
void init_models();
void draw_models();
void teardown_models();
#else
void init_models() {}
void draw_models() {}
void teardown_models() {}
#endif

}   // t_mob
