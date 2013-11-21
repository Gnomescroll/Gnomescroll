/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
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
