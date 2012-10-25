#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <item/config/item_drop_alt.hpp>

namespace t_map
{

extern class Item::ItemDropConfig* block_drop_dat;

void init_block_drop_dat();
void teardown_block_drop_dat();
void load_block_drop_dat();

void handle_block_drop(int x, int y, int z, CubeID cube_id);

}   // t_map
