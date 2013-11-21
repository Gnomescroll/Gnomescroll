/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#if DC_SERVER
# error Do not include this file in the server
#endif

namespace Animations
{

void init_block_damage();
void render_block_damage();

extern int predicted_block_damage;
extern bool damaging_block;

}   // Animations
