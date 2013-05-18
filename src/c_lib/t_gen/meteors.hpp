#pragma once

#if DC_CLIENT
# error Don't include this file in the client
#endif

namespace t_gen
{

void meteor_fall();
void meteor_shower();
void degenerate_column(int x, int y, int z);

}   // t_gen
