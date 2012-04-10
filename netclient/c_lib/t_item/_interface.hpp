#pragma once



#ifdef DC_CLIENT
#include <c_lib/t_item/client/_interface.hpp>
#endif

#ifdef DC_SERVER
#include <c_lib/t_item/server/_interface.hpp>
#endif

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
