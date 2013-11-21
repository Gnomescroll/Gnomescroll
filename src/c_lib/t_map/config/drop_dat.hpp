/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#if DC_SERVER
# include <item/config/item_drop_alt.hpp>
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

void handle_block_drop(const Vec3i& position, CubeType cube_type);
#endif

void init_block_drop_dat();
void teardown_block_drop_dat();
void load_block_drop_dat();

}   // t_map
