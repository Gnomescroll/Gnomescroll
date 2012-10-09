#pragma once

#include <item/common/enum.hpp>

namespace ItemContainer
{

void init();
void teardown();

void init_packets();

class ItemContainerInterface* get_container(int id);
ItemContainerType get_container_type(int container_id);
int get_container_owner(int container_id);
void destroy_container(int container_id);
void container_block_destroyed(int container_id, int x, int y, int z);

bool container_block_in_range_of(Vec3 pos, int block[3]);

}   // ItemContainer

//CLIENT
#if DC_CLIENT
namespace ItemContainer
{

class ItemContainerInterface* create_container(ItemContainerType type, int id);

void update_container_ui_from_state();

void update_smelter_ui();

bool open_container(int container_id);
bool close_container(int container_id);

void open_inventory();
void close_inventory();

int get_event_container_id(int event_id);
class ItemContainerUIInterface* get_container_ui(int container_id);
ItemID get_toolbelt_item(int slot);

ItemID* get_container_contents(int container_id);
int get_sprite_index_for_id(ItemID item_id);
int get_sprite_index_for_type(int item_type);
int get_container_ui_slot_max(int container_id);
int* get_container_ui_types(int container_id);
int* get_container_ui_stacks(int container_id);

void set_ui_slot_durability(int container_id, int slot, int durability);
void set_ui_slot_stack_size(int container_id, int slot, int stack_size);

}   // ItemContainer
#endif 

//SERVER
#if DC_SERVER
namespace ItemContainer
{

bool agent_can_access_container(int agent_id, int container_id);

ItemID get_agent_hand_item(int agent_id);
int get_agent_hand(int agent_id);
int get_agent_inventory(int agent_id);
int get_agent_toolbelt(int agent_id);
int get_agent_synthesizer(int agent_id);
int get_agent_energy_tanks(int agent_id);

ItemID get_agent_toolbelt_item(int agent_id, int slot);

ItemContainerInterface* create_container(ItemContainerType type);

void assign_containers_to_agent(int agent_id, int client_id);

void agent_born(int agent_id);
void agent_died(int agent_id);
void agent_quit(int agent_id);

void purchase_item_from_synthesizer(int agent_id, int shopping_slot);

void craft_item_from_bench(int agent_id, int container_id, int craft_slot);
bool consume_crafting_reagents(int agent_id, int container_id, int recipe_id);

void send_container_contents(int agent_id, int client_id, int container_id);

bool agent_in_container_range(int agent_id, int container_id);
void check_agents_in_container_range(); // checks that any agent accessing a container is still in range

// for brand new items going to a container
ContainerActionType auto_add_free_item_to_container(int client_id, int container_id, ItemID item_id);

// definition in server.hpp
void send_container_remove(int client_id, int container_id, int slot);

void update_smelters();

// used by serializer; places an item into a container based on the item's location information
// returns false on error
bool load_item_into_container(ItemID item_id, int container_id, int container_slot);
bool load_item_into_hand(ItemID item_id, int agent_id);

// tests
void test_container_list_capacity();

}   // ItemContainer
#endif


