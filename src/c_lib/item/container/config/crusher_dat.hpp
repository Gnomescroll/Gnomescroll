#pragma once

#if DC_CLIENT
# error Don't include this file in the client
#endif

namespace ItemContainer
{

void init_crusher_dat();
void teardown_crusher_dat();

void load_crusher_dat();

float crusher_item_jump_out_velocity();
class Item::ItemDrop* get_crusher_drop(ItemType item_type);

}   // ItemContainer
