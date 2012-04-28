#pragma once

typedef int ItemID;

namespace Item
{

void init();
void teardown();
class ItemContainer* get_container(int id);

}

/*
CLIENT
*/
#if DC_CLIENT

namespace Item
{

class Item* create_item(int item_type, ItemID item_id);
class Item* create_item_particle(int item_type, ItemID item_id, float x, float y, float z, float vx, float vy, float vz);

void mouse_right_click_handler(int id, int slot);
void mouse_left_click_handler(int id, int slot);

ItemID* get_container_contents(int container_id);
int get_sprite_index(int item_id);
}

#endif 



/*
SERVER
*/

#if DC_SERVER

namespace Item
{

void assign_container_to_agent(int agent_id, int client_id);

class Item* create_item(int item_type);
class Item* create_item_particle(int item_type, float x, float y, float z, float vx, float vy, float vz);

void check_item_pickups();
}
#endif


