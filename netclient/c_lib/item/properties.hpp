#pragma once

namespace t_item
{

extern int sprite_array[MAX_ITEMS];
extern int type_array[MAX_ITEMS];
extern class ItemAttribute* item_attribute_array;

void init_properties();

class ItemAttribute* get_item_attributes(int item_type);

int get_sprite_index_for_id(ItemID id);
int get_sprite_index_for_type(int type);

void set_item_name(int id, char* name, int length);
char* get_item_name(int id);
int get_item_id(char* name);

int get_item_group(int item_type);
}
