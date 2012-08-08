#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

namespace ItemContainer
{

void init_recycler_dat();
void teardown_recycler_dat();

void load_recycler_dat();


float recycler_item_jump_out_velocity();
class Item::ItemDrop* get_recycler_drop(int item_type);

}   // ItemContainer
