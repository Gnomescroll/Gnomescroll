#pragma once

#include <item/common/enum.hpp>

namespace Item
{

void init();
void teardown();

void init_packets();

class Item* get_item(ItemID id);

int get_item_type(ItemID id);
ItemGroup get_item_group(ItemID id);
int get_stack_size(ItemID id);  // space used in a stack
int get_stack_space(ItemID id); // space left in a stack
int get_item_durability(ItemID id);
int get_item_charges(ItemID id);

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

class Item* create_item(int item_type);
class Item* create_item(const char* item_name);

void destroy_item(ItemID id);
void destroy_item_for_loading(ItemID id);   // only used by serializer

ItemID split_item_stack(ItemID src, int amount);
ItemID split_item_stack_in_half(ItemID src);

unsigned int item_space();

// returns remaining stack size
int consume_stack_item(ItemID item_id); // automatically destroys consumed item
int consume_stack_item(ItemID item_id, int amount); // automatically destroys consumed item
int consume_stack_item(ItemID item_id, int amount, bool auto_destroy);

// returns remaining durability
int consume_durability(ItemID item_id); // automatically destroys consumed item
int consume_durability(ItemID item_id, int amount); // automatically destroys consumed item
int consume_durability(ItemID item_id, int amount, bool auto_destroy);

void agent_quit(AgentID agent_id);

// tests
void test_item_list_capacity();

void tick();

}   // Item
#endif


