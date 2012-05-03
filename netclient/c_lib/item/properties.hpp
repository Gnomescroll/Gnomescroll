#pragma once

#include <item/common/constant.hpp>

namespace Item
{

extern int sprite_array[MAX_ITEMS];
extern int type_array[MAX_ITEMS];
extern int group_array[MAX_ITEMS];
extern class ItemAttribute* item_attribute_array;

void init_properties();

class ItemAttribute* get_item_attributes(int item_type);
int get_item_fire_rate(int item_type);

int get_sprite_index_for_id(ItemID id);
int get_sprite_index_for_type(int type);

void set_item_name(int id, char* name, int length);
void set_item_name(int id, char* name);

char* get_item_name(int id);
int get_item_id(char* name);

int get_item_group_for_type(int item_type);

int dat_get_item_id(const char* name);

int get_max_stack_size(int item_type);
int get_max_energy(int item_type);
int get_max_durability(int item_type);

}
