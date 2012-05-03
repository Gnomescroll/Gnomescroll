#pragma once

#include <item/common/enum.hpp>

namespace Item
{

void init();
void teardown();
class ItemContainer* get_container(int id);
class Item* get_item(ItemID id);
class Item* get_item_object(ItemID id); // alias for get_item
//class Item* get_or_create_item(int item_type, ItemID item_id);
int get_item_type(ItemID id);
int get_item_group(ItemID id);
int get_stack_size(ItemID id);  // space used in a stack
int get_stack_space(ItemID id); // space left in a stack
int get_item_durability(ItemID id);

void destroy_item(ItemID id);

void merge_item_stack(ItemID src, ItemID dest);
void merge_item_stack(ItemID src, ItemID dest, int amount);

}

/*
CLIENT
*/
#if DC_CLIENT
namespace Item
{

void update_container_ui_from_state();

void open_container();
void close_container();

int get_event_container_id(int event_id);
class ItemContainerUI* get_container_ui(int container_id);
ItemID get_toolbelt_item(int slot);

class Item* create_item(int item_type, ItemID item_id);

void mouse_right_click_handler(int id, int slot);
void mouse_left_click_handler(int id, int slot);

ItemID* get_container_contents(int container_id);
int get_sprite_index_for_id(ItemID item_id);
int get_sprite_index_for_type(int item_type);
int* get_container_ui_types(int container_id);
int* get_container_ui_stacks(int container_id);

void set_ui_slot_durability(int container_id, int slot, int durability);
}
#endif 



/*
SERVER
*/

#if DC_SERVER
namespace Item
{

ItemID split_item_stack(ItemID src, int amount);
ItemID split_item_stack_in_half(ItemID src);
bool agent_owns_container(int agent_id, int container_id);

ItemID get_agent_hand(int agent_id);
int get_agent_container(int agent_id);
int get_agent_toolbelt(int agent_id);
ItemID get_agent_toolbelt_item(int agent_id, int slot);
void assign_container_to_agent(int agent_id, int client_id);

class Item* create_item(int item_type);

}
#endif


