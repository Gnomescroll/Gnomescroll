#pragma once

#if DC_SERVER
#include <item/config/item_drop_alt.hpp>
#endif

namespace t_map
{

typedef enum
{
    DROP_UNDEFINED,
    DROP_DEFINED,
    DROP_NEVER
}   DropDefinition;

#if DC_SERVER
extern class Item::ItemDropConfig* block_drop_dat;

void handle_block_drop(int x, int y, int z, CubeID cube_id);
#endif

void init_block_drop_dat();
void teardown_block_drop_dat();
void load_block_drop_dat();


}   // t_map
