#pragma once

namespace t_item
{

extern int sprite_array[MAX_ITEMS];
extern int type_array[MAX_ITEMS];

void init_properties();

int get_sprite_index_for_id(ItemID id);
int get_sprite_index_for_type(int type);

void set_item_name(int id, char* name, int length);
char* get_item_name(int id);
int get_item_id(char* name);

}
