#pragma once



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

void mouse_right_click_handler(int id, int slot);
void mouse_left_click_handler(int id, int slot);

}

#endif 



/*
SERVER
*/

#if DC_SERVER

namespace Item
{

void check_item_pickups();

void create_item(int inventory_id);
}
#endif


