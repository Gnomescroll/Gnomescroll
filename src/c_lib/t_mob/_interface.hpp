#pragma once

namespace t_mob
{

#if GS_ASSIMP
void init();
void draw();
void teardown();
#else
void init() {}
void draw() {}
void teardown() {}
#endif

}   // t_mob
