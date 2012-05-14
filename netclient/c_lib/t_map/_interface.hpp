#pragma once

namespace t_map
{
void end_t_map();

int get_block_item_container(int x, int y, int z);

#if DC_CLIENT
extern class Vbo_map* vbo_map;

void init_t_vbo();
void end_t_vbo();

void draw_map();
void update_map();
#endif

}
