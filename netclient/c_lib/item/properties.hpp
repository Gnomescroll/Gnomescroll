#pragma once

namespace t_item
{


int get_sprite_index_for_id(ItemID id);
int get_sprite_index_for_type(int type);

void set_item_name(int id, char* name, int length);
char* get_item_name(int id);
int get_item_id(char* name);

}
