#pragma once

#include <item/common/enum.hpp>
#include <item/common/constants.hpp>

namespace ItemContainer
{

const ItemContainerType player_container_types[N_PLAYER_CONTAINERS] = {
    AGENT_HAND,
    AGENT_TOOLBELT,
    AGENT_INVENTORY,
    AGENT_SYNTHESIZER,
    AGENT_ENERGY_TANKS
};

void init();
void teardown();

void init_packets();

class ItemContainerInterface* get_container(int id);
ItemContainerType get_container_type(int container_id);
int get_container_owner(int container_id);
void destroy_container(int container_id);
void container_block_destroyed(int container_id, int x, int y, int z);

bool container_block_in_range_of(struct Vec3 pos, int block[3]);

}   // ItemContainer

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

#if DC_SERVER
namespace ItemContainer
{

bool agent_can_access_container(AgentID agent_id, int container_id);

ItemID get_agent_hand_item(AgentID agent_id);
int get_agent_hand(AgentID agent_id);
int get_agent_inventory(AgentID agent_id);
int get_agent_toolbelt(AgentID agent_id);
int get_agent_synthesizer(AgentID agent_id);
int get_agent_energy_tanks(AgentID agent_id);

// returns array of assigned container ids for agent
// the array does not need to be freed and is valid until the next call to this method 
int* get_player_containers(AgentID agent_id, int* n_containers);

ItemID get_agent_toolbelt_item(AgentID agent_id, int slot);

ItemContainerInterface* create_container(ItemContainerType type);

bool assign_containers_to_agent(AgentID agent_id, ClientID client_id);
void send_container_assignments_to_agent(AgentID agent_id, ClientID client_id);

void agent_born(AgentID agent_id);
void agent_died(AgentID agent_id);
void agent_quit(AgentID agent_id);

void purchase_item_from_synthesizer(AgentID agent_id, int shopping_slot);

void craft_item_from_bench(AgentID agent_id, int container_id, int craft_slot);
bool consume_crafting_reagents(AgentID agent_id, int container_id, int recipe_id);

void send_container_contents(AgentID agent_id, ClientID client_id, int container_id);

bool agent_in_container_range(AgentID agent_id, int container_id);
void check_agents_in_container_range(); // checks that any agent accessing a container is still in range

// for brand new Item*s going to a container
ContainerActionType auto_add_free_item_to_container(ClientID client_id, int container_id, ItemID item_id);

// creates and adds item to the container. return id of created item
// if you modify the item's state after this function returns, make sure to call
// Item::send_item_state(item_id);
ItemID auto_add_item_to_container(const char* item_name, int container_id);

// definition in server.hpp
void send_container_remove(ClientID client_id, int container_id, int slot);

void update_smelters();

// used by serializer; places an item into a container based on the item's location information
// returns false on error
bool load_item_into_container(ItemID item_id, int container_id, int container_slot);
bool load_item_into_hand(ItemID item_id, AgentID agent_id);

// tests
void test_container_list_capacity();

}   // ItemContainer
#endif


