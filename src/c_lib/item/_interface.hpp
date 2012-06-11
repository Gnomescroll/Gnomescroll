#pragma once

#include <item/common/enum.hpp>

namespace Item
{

void init();
void teardown();

class Item* get_item(ItemID id);
class Item* get_item_object(ItemID id); // alias for get_item
//class Item* get_or_create_item(int item_type, ItemID item_id);
int get_item_type(ItemID id);
ItemGroup get_item_group(ItemID id);
int get_stack_size(ItemID id);  // space used in a stack
int get_stack_space(ItemID id); // space left in a stack
int get_item_durability(ItemID id);

void merge_item_stack(ItemID src, ItemID dest); // Caller of this function is expected to destroy the src
void merge_item_stack(ItemID src, ItemID dest, int amount);
}   // Item

//CLIENT
#if DC_CLIENT
namespace Item
{
void destroy_item(ItemID id);
class Item* create_item(int item_type, ItemID item_id);
}   // Item
#endif 

//SERVER
#if DC_SERVER
namespace Item
{

void destroy_item(ItemID id);
//void destroy_item_silently(ItemID id);  // wont broadcast
    
ItemID split_item_stack(ItemID src, int amount);
ItemID split_item_stack_in_half(ItemID src);

class Item* create_item(int item_type);
class Item* create_item(char* item_name);

// returns stack size
int consume_stack_item(ItemID item_id);

void agent_quit(int agent_id);

// tests
void test_item_list_capacity();

}   // Item
#endif


