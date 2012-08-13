#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

namespace Animations
{

void init_block_damage();
void render_block_damage();

extern int predicted_block_damage;
extern bool damaging_block;

}   // Animations
