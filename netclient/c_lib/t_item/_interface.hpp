#pragma once



namespace t_item
{


extern class Free_item_list* free_item_list;
extern class ItemContainerList* item_container_list;
extern class ItemList* item_list;

void state_init();
void state_teardown();

void tick();
void draw();

void check_item_pickups();

void create_free_item(int item_type, 
	float x, float y, float z,
 float vx, float vy, float vz);


}

/*
CLIENT
*/
#ifdef DC_CLIENT

namespace t_item
{
	extern int player_inventory_id;

}

#endif 



/*
SERVER
*/
#ifdef DC_SERVER

namespace t_item
{

void check_item_pickups();

void create_free_item(int item_type, 
    float x, float y, float z, 
    float vx, float vy, float vz);

}

#endif 
