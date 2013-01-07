#pragma once

#include <item/common/enum.hpp>
#include <item/common/constants.hpp>

namespace ItemContainer
{

ItemContainerType player_container_types[N_PLAYER_CONTAINERS] = { NULL_CONTAINER_TYPE };

void init();
void teardown();

void init_packets();

class ItemContainerInterface* get_container(ItemContainerID id);
ItemContainerType get_container_type(ItemContainerID container_id);
AgentID get_container_owner(ItemContainerID container_id);
void destroy_container(ItemContainerID container_id);

bool container_block_in_range_of(struct Vec3 pos, int block[3]);

}   // ItemContainer

#if DC_CLIENT
namespace ItemContainer
{

class ItemContainerInterface* create_container(ItemContainerType type, ItemContainerID id);

void update_container_ui_from_state();

void update_smelter_ui();

bool open_container(ItemContainerID container_id);
bool close_container(ItemContainerID container_id);

void open_inventory();
void close_inventory();

ItemContainerID get_event_container_id(int event_id);
class ItemContainerUIInterface* get_container_ui(ItemContainerID container_id);
ItemID get_toolbelt_item(int slot);

ItemID* get_container_contents(ItemContainerID container_id);
int get_sprite_index_for_id(ItemID item_id);
int get_sprite_index_for_type(int item_type);
int get_container_ui_slot_max(ItemContainerID container_id);

void set_ui_slot_durability(ItemContainerID container_id, int slot, int durability);
void set_ui_slot_stack_size(ItemContainerID container_id, int slot, int stack_size);

}   // ItemContainer
#endif 

#if DC_SERVER
namespace ItemContainer
{

bool agent_can_access_container(AgentID agent_id, ItemContainerID container_id);

ItemID get_agent_hand_item(AgentID agent_id);
ItemContainerID get_agent_hand(AgentID agent_id);
ItemContainerID get_agent_inventory(AgentID agent_id);
ItemContainerID get_agent_toolbelt(AgentID agent_id);
ItemContainerID get_agent_synthesizer(AgentID agent_id);
ItemContainerID get_agent_energy_tanks(AgentID agent_id);

// returns array of assigned container ids for agent
// the array does not need to be freed and is valid until the next call to this method 
ItemContainerID* get_player_containers(AgentID agent_id, size_t* n_containers);

ItemID get_agent_toolbelt_item(AgentID agent_id, int slot);

ItemContainerInterface* create_container(ItemContainerType type);

bool assign_containers_to_agent(AgentID agent_id, ClientID client_id);
void send_container_assignments_to_agent(AgentID agent_id, ClientID client_id);

void agent_born(AgentID agent_id);
void agent_died(AgentID agent_id);
void agent_quit(AgentID agent_id);

void purchase_item_from_synthesizer(AgentID agent_id, int shopping_slot);

void craft_item_from_bench(AgentID agent_id, ItemContainerID container_id, int craft_slot);
bool consume_crafting_reagents(AgentID agent_id, ItemContainerID container_id, int recipe_id);

void send_container_contents(AgentID agent_id, ClientID client_id, ItemContainerID container_id);

bool agent_in_container_range(AgentID agent_id, ItemContainerID container_id);
void check_agents_in_container_range(); // checks that any agent accessing a container is still in range

// for brand new Item*s going to a container
ContainerActionType auto_add_free_item_to_container(ClientID client_id, ItemContainerID container_id, ItemID item_id);

// creates and adds item to the container. return id of created item
// if you modify the item's state after this function returns, make sure to call
// Item::send_item_state(item_id);
ItemID auto_add_item_to_container(const char* item_name, ItemContainerID container_id);

// Create a container at a map location and returns the container ID
// Remember to broadcast_container_create(id) if you are calling this
// outside of the init phase (when players may be playing)
ItemContainerID create_container_block(const char* container_name, int x, int y, int z);
ItemContainerID create_container_block(ItemContainerType container_type, int x, int y, int z);

// definition in server.hpp
void send_container_remove(ClientID client_id, ItemContainerID container_id, int slot);

void update_smelters();

// used by serializer; places an item into a container based on the item's location information
// returns false on error
bool load_item_into_container(ItemID item_id, ItemContainerID container_id, int container_slot);
bool load_item_into_hand(ItemID item_id, AgentID agent_id);

void container_block_destroyed(ItemContainerID container_id, int x, int y, int z);

// tests
void test_container_list_capacity();

}   // ItemContainer
#endif


