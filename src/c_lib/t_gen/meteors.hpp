/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#if DC_CLIENT
# error Do not include this file in the client
#endif

namespace t_gen
{

void meteor_fall();
void meteor_shower();
void degenerate_column(int x, int y, int z);

}   // t_gen
